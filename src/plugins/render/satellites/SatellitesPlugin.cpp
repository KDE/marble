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

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "ViewportParams.h"

#include "GeoPainter.h"
#include "GeoDataLinearRing.h"

#include "sgp4/sgp4io.h"

#include <locale.h>

namespace Marble
{

SatellitesPlugin::SatellitesPlugin()
    : m_isInitialized(false)
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
    //marbleModel()->downloadManager();
    //Data from http://www.celestrak.com/NORAD/elements/
    QFile tleFile("/opt/geo.txt");
    if (!tleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        mDebug() << "Error opening file";
        return;
    }

    //FIXME: terrible hack because twoline2rv uses sscanf
    setlocale(LC_NUMERIC, "C");

    double startmfe, stopmfe, deltamin;
    elsetrec satrec;
    while(!tleFile.atEnd()) {
        QString satellite(tleFile.readLine());
        char line1[80];
        char line2[80];
        tleFile.readLine(line1, sizeof(line1));
        tleFile.readLine(line2, sizeof(line2));
        twoline2rv(line1, line2, 'c', 'd', 'i', wgs84,
                   startmfe, stopmfe, deltamin, satrec);
        m_satHash.insert(satellite.trimmed(), satrec);
        if (satrec.error != 0) {
            mDebug() << "Error: " << satrec.error;
            return;
        }
    }
    setlocale(LC_NUMERIC, ""); //reset to environment
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
            orbit << fromCartesian( r[0], r[1], r[2]);
        }

        painter->save();

        painter->setPen( oxygenBrickRed4 );
        painter->setBrush( Qt::NoBrush );
        painter->drawPolygon( orbit );

        painter->restore();
    }
    return true;
}

GeoDataCoordinates SatellitesPlugin::fromCartesian( double x, double y, double z ) {
    double lat = atan2( y, x );
    double lon = atan2( z, sqrt( x*x + y*y ) );
    double alt = sqrt( x*x + y*y + z*z );
    return GeoDataCoordinates( lat, lon, alt*1000 - marbleModel()->planetRadius() );
}

}

Q_EXPORT_PLUGIN2(SatellitesPlugin, Marble::SatellitesPlugin)

#include "SatellitesPlugin.moc"
