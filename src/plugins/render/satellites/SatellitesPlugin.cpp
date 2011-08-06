//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "SatellitesPlugin.h"

#include <QtCore/QTimer>

#include <locale.h>

#include "CacheStoragePolicy.h"
#include "HttpDownloadManager.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "ViewportParams.h"

#include "GeoPainter.h"
#include "GeoDataLinearRing.h"

#include "sgp4/sgp4io.h"

const int timeBetweenDownloads = 6*60*60*1000; //in milliseconds

namespace Marble
{

SatellitesPlugin::SatellitesPlugin()
    : m_isInitialized(false),
      m_downloadTimer(new QTimer(this))
{
}

QStringList SatellitesPlugin::backendTypes() const
{
    return QStringList( "satellites" );
}

QString SatellitesPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList SatellitesPlugin::renderPosition() const
{
    return QStringList( "ORBIT" );
}

QString SatellitesPlugin::name() const
{
    return tr( "Satellites" );
}

QString SatellitesPlugin::guiString() const
{
    return tr( "&Satellites" );
}

QString SatellitesPlugin::nameId() const
{
    return QString( "satellites-plugin" );
}

QString SatellitesPlugin::description() const
{
    return tr( "This plugin displays satellites and their orbits." );
}

QIcon SatellitesPlugin::icon() const
{
    return QIcon();
}

void SatellitesPlugin::initialize()
{
    CacheStoragePolicy *storagePolicy = new CacheStoragePolicy( MarbleDirs::localPath()
                                                                + "/cache/satellites/" );
    m_downloadManager = new HttpDownloadManager( storagePolicy, marbleModel()->pluginManager() );

    connect( m_downloadManager, SIGNAL(downloadComplete(QByteArray,QString)),
             this, SLOT(updateData(QByteArray,QString)) );
    connect( m_downloadTimer, SIGNAL(timeout()),
             this, SLOT(triggerDownload()) ) ;

    m_downloadTimer->start( timeBetweenDownloads );
    triggerDownload();
    m_isInitialized = true;
}

bool SatellitesPlugin::isInitialized() const
{
    return m_isInitialized;
}

bool SatellitesPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    if ( marbleModel()->planetId() != "earth" ) {
        return true;
    }

    painter->autoMapQuality();
    painter->setPen( oxygenSkyBlue4 );
    painter->setBrush( oxygenSkyBlue4 );

    QHash<QString, elsetrec>::iterator it = m_satHash.begin();
    QHash<QString, elsetrec>::iterator end = m_satHash.end();
    for (; it != end; ++it ) {
        double r[3], v[3], polar[3];
        sgp4( wgs84, it.value(), 0, r, v );
        if (it.value().error != 0) {
            mDebug() << "Error: " << it.value().error;
            return false;
        }
        mDebug() << "pos: " << r[0] << " " << r[1] << " " << r[2];
        mDebug() << "speed: " << v[0] << " " << v[1] << " " << v[2];

        GeoDataCoordinates satCoords = fromCartesian( r[0], r[1], r[2] );
        painter->drawRect( satCoords, 15, 15 );
        mDebug() << satCoords.altitude();

        painter->save();

        painter->setPen( QColor( Qt::white ) );
        painter->drawText( satCoords, it.key() );

        painter->restore();

        GeoDataLinearRing orbit;
        for ( int i = 1; i < 100; i++ ) {
            sgp4( wgs84, it.value(), i, r, v );
            orbit << fromCartesian( r[0], r[1], r[2] );
        }

        painter->save();

        painter->setPen( oxygenBrickRed4 );
        painter->setBrush( Qt::NoBrush );
        painter->drawPolygon( orbit );

        painter->restore();
    }
    return true;
}

void SatellitesPlugin::updateData(const QByteArray &data, const QString &initiatorId)
{
    //FIXME: terrible hack because twoline2rv uses sscanf
    setlocale(LC_NUMERIC, "C");

    QList<QByteArray> tleLines = data.split( '\n' );
    double startmfe, stopmfe, deltamin;
    elsetrec satrec;
    int i = 0;
    // File format: One line of description, two lines of TLE, last line is empty
    if ( tleLines.size() % 3 != 1 ) {
        mDebug() << "Malformated satellite data file: " << initiatorId;
    }
    while ( i < tleLines.size() - 1 ) {
        QString satelliteName( tleLines.at( i++ ) );
        char line1[80];
        char line2[80];
        qstrcpy( line1, tleLines.at( i++ ).constData() );
        qstrcpy( line2, tleLines.at( i++ ).constData() );
        twoline2rv( line1, line2, 'c', 'd', 'i', wgs84,
                    startmfe, stopmfe, deltamin, satrec );
        m_satHash.insert( satelliteName.trimmed(), satrec );
        if ( satrec.error != 0 ) {
            mDebug() << "Error: " << satrec.error;
            return;
        }
    }

    repaintNeeded(QRegion());

    //Reset to environment
    setlocale(LC_NUMERIC, "");
}

void SatellitesPlugin::triggerDownload()
{
    m_downloadManager->addJob( QUrl( "http://www.celestrak.com/NORAD/elements/visual.txt" ),
                                              "visual.txt", "sat-visual", Marble::DownloadBulk );
}

GeoDataCoordinates SatellitesPlugin::fromCartesian( double x, double y, double z ) {
    double lat = atan2( y, x );
    double lon = atan2( z, sqrt( x*x + y*y ) );
    double alt = sqrt( x*x + y*y + z*z );
    return GeoDataCoordinates( lat, lon, alt*1000 - marbleModel()->planetRadius() );
}

}

Q_EXPORT_PLUGIN2( SatellitesPlugin, Marble::SatellitesPlugin )

#include "SatellitesPlugin.moc"
