//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "MonavPlugin.h"

#include "signals.h"
#include "MonavRunner.h"
#include "MonavConfigWidget.h"
#include "MonavMap.h"
#include "MonavMapsModel.h"

#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "GeoDataLatLonBox.h"
#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "routing/RouteRequest.h"

#include <QProcess>
#include <QDirIterator>
#include <QLocalSocket>
#include <QThread>
#include <QTextStream>

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

    QString m_monavDaemonProcess;

    MonavPlugin::MonavRoutingDaemonVersion m_monavVersion;

    MonavPluginPrivate();

    ~MonavPluginPrivate();

    bool startDaemon();

    void stopDaemon();

    static bool isDaemonRunning();

    static bool isDaemonInstalled();

    void loadMaps();

    void initialize();

    static bool areaLessThan( const MonavMap &first, const MonavMap &second );

private:
    void loadMap( const QString &path );

    bool m_initialized;
};

MonavPluginPrivate::MonavPluginPrivate() : m_ownsServer( false ),
    m_monavDaemonProcess("monav-daemon"), m_monavVersion( MonavPlugin::Monav_0_3 ),
    m_initialized( false )
{
    // nothing to do
}

MonavPluginPrivate::~MonavPluginPrivate()
{
    stopDaemon();
}

bool MonavPluginPrivate::isDaemonRunning()
{
    QLocalSocket socket;
    socket.connectToServer( "MoNavD" );
    return socket.waitForConnected();
}

bool MonavPluginPrivate::isDaemonInstalled()
{
    QString path = QProcessEnvironment::systemEnvironment().value(QStringLiteral("PATH"), QStringLiteral("/usr/local/bin:/usr/bin:/bin"));
    foreach( const QString &application, QStringList() << "monav-daemon" << "MoNavD" ) {
        foreach( const QString &dir, path.split( QLatin1Char( ':' ) ) ) {
            QFileInfo executable( QDir( dir ), application );
            if ( executable.exists() ) {
                return true;
            }
        }
    }

    return false;
}

bool MonavPluginPrivate::startDaemon()
{
    if ( !isDaemonRunning() ) {
        QProcess process;
        if ( process.startDetached( m_monavDaemonProcess ) ) {
            m_ownsServer = true;
        } else {
            if ( process.startDetached( "MoNavD" ) ) {
                m_ownsServer = true;
                m_monavDaemonProcess = "MoNavD";
                m_monavVersion = MonavPlugin::Monav_0_2;
            } else {
                return false;
            }
        }

        // Give monav-daemon up to one second to set up its server
        // Without that, the first route request would fail
        for ( int i = 0; i < 10; ++i ) {
            if ( isDaemonRunning() ) {
                break;
            }
            MonavWaiter::msleep( 100 );
        }

        return true;
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
        process.startDetached( m_monavDaemonProcess, QStringList() << "-t" );
    }
}

void MonavPluginPrivate::loadMaps()
{
    if ( m_maps.isEmpty() ) {
        QStringList const baseDirs = QStringList() << MarbleDirs::systemPath() << MarbleDirs::localPath();
        foreach ( const QString &baseDir, baseDirs ) {
            const QString base = baseDir + QLatin1String("/maps/earth/monav/");
            loadMap( base );
            QDir::Filters filters = QDir::AllDirs | QDir::Readable | QDir::NoDotAndDotDot;
            QDirIterator::IteratorFlags flags = QDirIterator::Subdirectories | QDirIterator::FollowSymlinks;
            QDirIterator iter( base, filters, flags );
            while ( iter.hasNext() ) {
                iter.next();
                loadMap( iter.filePath() );
            }
        }
        // Prefer maps where bounding boxes are known
        qSort( m_maps.begin(), m_maps.end(), MonavMap::areaLessThan );
    }
}

void MonavPluginPrivate::loadMap( const QString &path )
{
    QDir mapDir( path );
    QFileInfo pluginsFile( mapDir, "plugins.ini" );
    QFileInfo moduleFile( mapDir, "Module.ini" );
    if ( pluginsFile.exists() && !moduleFile.exists() ) {
        qDebug() << "Migrating" << mapDir.dirName() << "from monav-0.2";
        QFile file( moduleFile.absoluteFilePath() );
        file.open( QIODevice::WriteOnly );
        QTextStream stream( &file );
        stream << "[General]\nconfigVersion=2\n";
        stream << "router=Contraction Hierarchies\ngpsLookup=GPS Grid\n";
        stream << "routerFileFormatVersion=1\ngpsLookupFileFormatVersion=1\n";
        stream.flush();
        file.close();
        moduleFile.refresh();
    }

    if ( moduleFile.exists() ) {
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

MonavPlugin::MonavPlugin( QObject *parent ) :
    RoutingRunnerPlugin( parent ),
    d( new MonavPluginPrivate )
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
    setCanWorkOffline( true );

    if ( d->isDaemonInstalled() ) {
        d->initialize();
        if ( d->m_maps.isEmpty() ) {
            setStatusMessage( tr ( "No offline maps installed yet." ) );
        }
    } else {
        setStatusMessage( tr ( "The monav routing daemon does not seem to be installed on your system." ) );
    }

    connect( qApp, SIGNAL(aboutToQuit()), this, SLOT(stopDaemon()) );
}

MonavPlugin::~MonavPlugin()
{
    delete d;
}

QString MonavPlugin::name() const
{
    return tr( "Monav Routing" );
}

QString MonavPlugin::guiString() const
{
    return tr( "Monav" );
}

QString MonavPlugin::nameId() const
{
    return QStringLiteral("monav");
}

QString MonavPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString MonavPlugin::description() const
{
    return tr( "Offline routing using the monav daemon" );
}

QString MonavPlugin::copyrightYears() const
{
    return QStringLiteral("2010");
}

QVector<PluginAuthor> MonavPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"));
}

RoutingRunner *MonavPlugin::newRunner() const
{
    d->initialize();
    if ( !d->startDaemon() ) {
        mDebug() << "Failed to start the monav routing daemon";
    }

    return new MonavRunner( this );
}

QString MonavPlugin::mapDirectoryForRequest( const RouteRequest* request ) const
{
    d->initialize();

    QHash<QString, QVariant> settings = request->routingProfile().pluginSettings()[nameId()];
    const QString transport = settings[QStringLiteral("transport")].toString();

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

QStringList MonavPlugin::mapDirectoriesForRequest( const RouteRequest* request ) const
{
    QStringList result;
    d->initialize();
    QHash<QString, QVariant> settings = request->routingProfile().pluginSettings()[nameId()];
    const QString transport = settings[QStringLiteral("transport")].toString();

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
            result << d->m_maps[j].directory().absolutePath();
        }
    }

    return result;
}

RoutingRunnerPlugin::ConfigWidget *MonavPlugin::configWidget()
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

bool MonavPlugin::canWork() const
{
    d->initialize();
    return !d->m_maps.isEmpty();
}

bool MonavPlugin::supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const
{
    // Since we support multiple maps, pretty much anything can be installed, but ecological is
    // not supported by monav
    return profileTemplate != RoutingProfilesModel::CarEcologicalTemplate;
}

QHash< QString, QVariant > MonavPlugin::templateSettings( RoutingProfilesModel::ProfileTemplate profileTemplate ) const
{
    QHash<QString, QVariant> result;
    switch ( profileTemplate ) {
        case RoutingProfilesModel::CarFastestTemplate:
            result["transport"] = "Motorcar";
            break;
        case RoutingProfilesModel::CarShortestTemplate:
            result["transport"] = "Motorcar";
            break;
        case RoutingProfilesModel::CarEcologicalTemplate:
            break;
        case RoutingProfilesModel::BicycleTemplate:
            result["transport"] = "Bicycle";
            break;
        case RoutingProfilesModel::PedestrianTemplate:
            result["transport"] = "Pedestrian";
            break;
        case RoutingProfilesModel::LastTemplate:
            Q_ASSERT( false );
            break;
    }
    return result;
}

MonavPlugin::MonavRoutingDaemonVersion MonavPlugin::monavVersion() const
{
    return d->m_monavVersion;
}

}

#include "moc_MonavPlugin.cpp"
