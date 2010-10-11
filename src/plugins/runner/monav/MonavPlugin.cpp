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
#include "GeoDataParser.h"
#include "GeoDataDocument.h"

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

    QVector<GeoDataLinearRing> m_tiles;

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

    static bool areaLessThan( const MonavMap &first, const MonavMap &second );

private:
    void loadMap( const QString &path );
};

void MonavMap::setDirectory( const QDir &dir )
{
    m_directory = dir;
    QFileInfo boundingBox( dir, dir.dirName() + ".kml" );
    if ( boundingBox.exists() ) {
        parseBoundingBox( boundingBox );
    } else {
        mDebug() << "No monav bounding box given for " << boundingBox.absoluteFilePath();
    }
}

void MonavMap::parseBoundingBox( const QFileInfo &file )
{
    GeoDataLineString points;
    QFile input( file.absoluteFilePath() );
    if ( input.open( QFile::ReadOnly ) ) {
        GeoDataParser parser( GeoData_KML );
        if ( !parser.read( &input ) ) {
            mDebug() << "Could not parse file: " << parser.errorString();
            return;
        }

        GeoDocument *doc = parser.releaseDocument();
        GeoDataDocument *document = dynamic_cast<GeoDataDocument*>( doc );
        QVector<GeoDataPlacemark*> placemarks = document->placemarkList();
        if ( placemarks.size() == 1 ) {
            GeoDataPlacemark* placemark = placemarks.first();
            GeoDataMultiGeometry* geometry = dynamic_cast<GeoDataMultiGeometry*>( placemark->geometry() );
            for ( int i=0; geometry && i<geometry->size(); ++i ) {
                GeoDataLinearRing* poly = dynamic_cast<GeoDataLinearRing*>( geometry->child( i ) );
                if ( poly ) {
                    for ( int j=0; j<poly->size(); ++j ) {
                        points << poly->at( j );
                    }
                    m_tiles.push_back( *poly );
                }
            }
        } else {
            mDebug() << "File " << file.absoluteFilePath() << " does not contain one placemark, but " << placemarks.size();
        }
    }
    m_boundingBox = points.latLonAltBox();
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

    // GeoDataLinearRing does a 3D check, but we only have 2D data for
    // the map bounding box. Therefore the 3D info of e.g. the GPS position
    // must be ignored.
    GeoDataCoordinates flatPosition = point;
    flatPosition.setAltitude( 0.0 );
    foreach( const GeoDataLinearRing &box, m_tiles ) {
        if ( box.contains( flatPosition ) ) {
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
    QDirIterator::IteratorFlags flags = QDirIterator::Subdirectories | QDirIterator::FollowSymlinks;
    QDirIterator iter( base, filters, flags );
    while ( iter.hasNext() ) {
        iter.next();
        loadMap( iter.filePath() );
    }
    // Prefer maps where bounding boxes are known
    qSort( m_maps.begin(), m_maps.end(), MonavPluginPrivate::areaLessThan );
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

bool MonavPluginPrivate::areaLessThan( const MonavMap &first, const MonavMap &second )
{
    if ( !first.m_tiles.isEmpty() && second.m_tiles.isEmpty() ) {
        return true;
    }

    if ( first.m_tiles.isEmpty() && !second.m_tiles.isEmpty() ) {
        return false;
    }

    qreal const areaOne = first.m_boundingBox.width() * first.m_boundingBox.height();
    qreal const areaTwo = second.m_boundingBox.width() * second.m_boundingBox.height();
    return areaOne < areaTwo;
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
    for ( int j=0; j<d->m_maps.size(); ++j ) {
        bool containsAllPoints = true;
        for ( int i = 0; i < request->size(); ++i ) {
            GeoDataCoordinates via = request->at( i );
            if ( !d->m_maps[j].containsPoint( via ) ) {
                containsAllPoints = false;
                break;
            }
        }

        if ( containsAllPoints ) {
            if ( j ) {
                // Subsequent route requests will likely be in the same country
                qSwap( d->m_maps[0], d->m_maps[j] );
            }
            // mDebug() << "Using " << d->m_maps.first().m_directory.dirName() << " as monav map";
            return d->m_maps.first().m_directory.absolutePath();
        }
    }

    return QString();
}

}

Q_EXPORT_PLUGIN2( MonavPlugin, Marble::MonavPlugin )

#include "MonavPlugin.moc"
