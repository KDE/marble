//
// This file is part of the Marble Desktop Globe.
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
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "GeoDataLatLonBox.h"

#include <QtCore/QProcess>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QTimer>
#include <QtNetwork/QLocalSocket>

#include <unistd.h>

namespace Marble
{

class MonavMap
{
public:
    QDir m_directory;

    GeoDataLatLonBox m_boundingBox;

    QVector<GeoDataLatLonBox> m_tiles;

    void setDirectory( const QDir &dir );

    bool containsPoint( const GeoDataCoordinates &point ) const;

private:
    void parseBoundingBox( const QFileInfo &file );
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

    static bool bordersFirst( const MonavMap &first, const MonavMap &second );

private:
    void loadMap( const QString &path );
};

void MonavMap::setDirectory( const QDir &dir )
{
    m_directory = dir;
    QFileInfo boundingBox( dir, dir.dirName() + ".poly" );
    if ( boundingBox.exists() ) {
        parseBoundingBox( boundingBox );
    } else {
        mDebug() << "No monav bounding box given for " << boundingBox.absoluteFilePath();
    }
}

void MonavMap::parseBoundingBox( const QFileInfo &file )
{
    QFile input( file.absoluteFilePath() );
    if ( input.open( QFile::ReadOnly ) ) {
        GeoDataLineString boundingBox;
        QTextStream stream( &input );
        stream.readLine();
        qreal lat( 0.0 ), lon( 0.0 );
        while ( !stream.atEnd() ) {
            if ( stream.readLine() == "END" ) {
                continue;
            }
            GeoDataLineString box;
            for ( int i = 0; i < 5; ++i ) {
                stream >> lon;
                stream >> lat;
                GeoDataCoordinates point( lon, lat, 0.0, GeoDataCoordinates::Degree );
                box << point;
                boundingBox << point;
            }
            m_tiles.append( box.latLonAltBox() );
            stream.readLine();
        }
        stream.readLine();
        m_boundingBox = boundingBox.latLonAltBox();
    }
}

bool MonavMap::containsPoint( const GeoDataCoordinates &point ) const
{
    // If we do not have a bounding box at all, we err on the safe side
    if ( m_tiles.isEmpty() ) {
        return true;
    }

    // Quick check for performance reasons
    if ( !m_boundingBox.contains( point ) ) {
        return false;
    }

    foreach( const GeoDataLatLonBox &box, m_tiles ) {
        if ( box.contains( point ) ) {
            return true;
        }
    }

    return false;
}

MonavPluginPrivate::MonavPluginPrivate() : m_ownsServer( false )
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
                usleep( 100 * 1000 );
            }

            return true;
        }

        return false;
    }

    return true;
}

void MonavPluginPrivate::stopDaemon()
{
    if ( m_ownsServer ) {
        m_ownsServer = false;
        QProcess process;
        process.startDetached( "MoNavD", QStringList() << "-t" );
    }
}

void MonavPluginPrivate::loadMaps()
{
    QString base = MarbleDirs::localPath() + "/maps/earth/monav/";
    loadMap( base );
    QDir::Filters filters = QDir::AllDirs | QDir::Readable | QDir::NoDotAndDotDot;
    QDirIterator iter( base, filters, QDirIterator::Subdirectories );
    while ( iter.hasNext() ) {
        iter.next();
        loadMap( iter.filePath() );
    }
    // Prefer maps where bounding boxes are known
    qSort( m_maps.begin(), m_maps.end(), MonavPluginPrivate::bordersFirst );
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

bool MonavPluginPrivate::bordersFirst( const MonavMap &first, const MonavMap &second )
{
    if ( !first.m_tiles.isEmpty() && second.m_tiles.isEmpty() ) {
        return true;
    }

    if ( first.m_tiles.isEmpty() && !second.m_tiles.isEmpty() ) {
        return false;
    }

    return first.m_directory.absolutePath() < second.m_directory.absolutePath();
}

MonavPlugin::MonavPlugin( QObject *parent ) : RunnerPlugin( parent ), d( new MonavPluginPrivate )
{
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( true );
    setName( tr( "Monav" ) );
    setNameId( "monav" );
    setDescription( tr( "Retrieves routes from monav" ) );
    setGuiString( tr( "Monav Routing" ) );

    // Check installation
    d->loadMaps();
    bool const haveMap = !d->m_maps.isEmpty();
    setCapabilities( haveMap ? Routing /*| ReverseGeocoding */ : None );
}

MonavPlugin::~MonavPlugin()
{
    delete d;
}

MarbleAbstractRunner* MonavPlugin::newRunner() const
{
    if ( !d->startDaemon() ) {
        mDebug() << "Failed to connect to MoNavD daemon";
    }

    return new MonavRunner( this );
}

QString MonavPlugin::mapDirectoryForRequest( RouteRequest* request ) const
{
    foreach( const MonavMap &map, d->m_maps ) {
        bool containsAllPoints = true;
        for ( int i = 0; i < request->size(); ++i ) {
            GeoDataCoordinates via = request->at( i );
            if ( !map.containsPoint( via ) ) {
                containsAllPoints = false;
                break;
            }
        }

        if ( containsAllPoints ) {
            return map.m_directory.absolutePath();
        }
    }

    return QString();
}

}

Q_EXPORT_PLUGIN2( MonavPlugin, Marble::MonavPlugin )

#include "MonavPlugin.moc"
