//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "signals.h"
#include "MonavPlugin.h"
#include "MonavRunner.h"
#include "MonavConfigWidget.h"
#include "MonavMap.h"
#include "MonavMapsModel.h"

#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "GeoDataLatLonBox.h"
#include "GeoDataParser.h"
#include "GeoDataDocument.h"
#include "GeoDataData.h"
#include "GeoDataExtendedData.h"

#include <QtCore/QProcess>
#include <QtCore/QDirIterator>
#include <QtCore/QTimer>
#include <QtNetwork/QLocalSocket>
#include <QtCore/QThread>

namespace Marble
{

/** A helper class to have a portable sleep call */
class MonavWaiter : private QThread
{
public:
    static void msleep( unsigned long milliSeconds ) {
        QThread::msleep( milliSeconds );
    }

private:
    MonavWaiter();
    Q_DISABLE_COPY( MonavWaiter )
};

class MonavPluginPrivate
{
public:
    QDir m_mapDir;

    QVector<MonavMap> m_maps;

    bool m_ownsServer;

    MonavPluginPrivate();

    ~MonavPluginPrivate();

    bool startDaemon();

    void stopDaemon();

    bool isDaemonRunning() const;

    void loadMaps();

    void initialize();

    static bool areaLessThan( const MonavMap &first, const MonavMap &second );

private:
    void loadMap( const QString &path );

    bool m_initialized;
};

MonavPluginPrivate::MonavPluginPrivate() : m_ownsServer( false ), m_initialized( false )
{
    // nothing to do
}

MonavPluginPrivate::~MonavPluginPrivate()
{
    stopDaemon();
}

bool MonavPluginPrivate::isDaemonRunning() const
{
    QLocalSocket socket;
    socket.connectToServer( "MoNavD" );
    return socket.waitForConnected();
}

bool MonavPluginPrivate::startDaemon()
{
    if ( !isDaemonRunning() ) {
        QProcess process;
        if ( process.startDetached( "MoNavD" ) ) {
            m_ownsServer = true;

            // Give MoNavD up to one second to set up its server
            // Without that, the first route request would fail
            for ( int i = 0; i < 10; ++i ) {
                if ( isDaemonRunning() ) {
                    break;
                }
                MonavWaiter::msleep( 100 );
            }

            return true;
        }

        return false;
    }

    return true;
}

void MonavPluginPrivate::stopDaemon()
{
    // So far Marble is the only application using monav-routingdaemon on Maemo.
    // Always shut down the monav server there, since monav-routingdaemon will
    // survive a crash of Marble and later block mounting a N900 via USB because
    // it has loaded some data from it. This way if Marble crashed and monav blocks
    // USB mounting (which the user will not be aware of), another start and shutdown
    // of Marble will at least fix the USB mount block.
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( smallScreen || m_ownsServer ) {
        m_ownsServer = false;
        QProcess process;
        process.startDetached( "MoNavD", QStringList() << "-t" );
    }
}

void MonavPluginPrivate::loadMaps()
{
    if ( m_maps.isEmpty() ) {
        QString base = MarbleDirs::localPath() + "/maps/earth/monav/";
        loadMap( base );
        QDir::Filters filters = QDir::AllDirs | QDir::Readable | QDir::NoDotAndDotDot;
        QDirIterator::IteratorFlags flags = QDirIterator::Subdirectories | QDirIterator::FollowSymlinks;
        QDirIterator iter( base, filters, flags );
        while ( iter.hasNext() ) {
            iter.next();
            loadMap( iter.filePath() );
        }
        // Prefer maps where bounding boxes are known
        qSort( m_maps.begin(), m_maps.end(), MonavMap::areaLessThan );
    }
}

void MonavPluginPrivate::loadMap( const QString &path )
{
    QDir mapDir( path );
    QFileInfo pluginsFile( mapDir, "plugins.ini" );
    if ( pluginsFile.exists() ) {
        MonavMap map;
        map.setDirectory( mapDir );
        m_maps.append( map );
    }
}

void MonavPluginPrivate::initialize()
{
    if ( !m_initialized ) {
        m_initialized = true;
        loadMaps();
    }
}

MonavPlugin::MonavPlugin( QObject *parent ) : RunnerPlugin( parent ), d( new MonavPluginPrivate )
{
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( true );
    setName( tr( "Monav" ) );
    setNameId( "monav" );
    setDescription( tr( "Retrieves routes from monav" ) );
    setGuiString( tr( "Monav Routing" ) );
    setCapabilities( Routing /*| ReverseGeocoding */ );
}

MonavPlugin::~MonavPlugin()
{
    delete d;
}

MarbleAbstractRunner* MonavPlugin::newRunner() const
{
    d->initialize();
    if ( !d->startDaemon() ) {
        mDebug() << "Failed to connect to MoNavD daemon";
    }

    return new MonavRunner( this );
}

QString MonavPlugin::mapDirectoryForRequest( RouteRequest* request ) const
{
    d->initialize();

    QHash<QString, QVariant> settings = request->routingProfile().pluginSettings()[nameId()];
    QString transport = settings["transport"].toString();

    for ( int j=0; j<d->m_maps.size(); ++j ) {
        bool valid = true;
        if ( transport.isEmpty() || transport == d->m_maps[j].transport() ) {
            for ( int i = 0; i < request->size(); ++i ) {
                GeoDataCoordinates via = request->at( i );
                if ( !d->m_maps[j].containsPoint( via ) ) {
                    valid = false;
                    break;
                }
            }
        } else {
            valid = false;
        }

        if ( valid ) {
            if ( j ) {
                // Subsequent route requests will likely be in the same country
                qSwap( d->m_maps[0], d->m_maps[j] );
            }
            // mDebug() << "Using " << d->m_maps.first().m_directory.dirName() << " as monav map";
            return d->m_maps.first().directory().absolutePath();
        }
    }

    return QString();
}

RunnerPlugin::ConfigWidget *MonavPlugin::configWidget()
{
    return new MonavConfigWidget( this );
}

MonavMapsModel* MonavPlugin::installedMapsModel()
{
    d->initialize();
    return new MonavMapsModel( d->m_maps );
}

void MonavPlugin::reloadMaps()
{
    d->m_maps.clear();
    d->loadMaps();
}

bool MonavPlugin::canWork( Capability capability ) const
{
    if ( supports( capability ) ) {
        d->initialize();
        return !d->m_maps.isEmpty();
    } else {
       return false;
    }
}

bool MonavPlugin::supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const
{
    // Since we support multiple maps, pretty much anything can be installed, but ecological is
    // not supported by monav
    return profileTemplate != RoutingProfilesModel::CarEcologicalTemplate;
}

}

Q_EXPORT_PLUGIN2( MonavPlugin, Marble::MonavPlugin )

#include "MonavPlugin.moc"
