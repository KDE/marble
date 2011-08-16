//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//


#include "SatellitesItem.h"

#include "MarbleDebug.h"
#include "global.h"
#include "GeoPainter.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLinearRing.h"

using namespace Marble;

SatellitesItem::SatellitesItem( const QString &name, const elsetrec &satrec, QObject *parent )
    : AbstractDataPluginItem( parent )
{
    setTarget( "earth" );
    //setSize( QSizeF( 50, 50 ) );
    setId( name );
    setToolTip( name );
    m_satrec = satrec;
    setVisible( true );

    double r[3], v[3];
    sgp4( wgs84, m_satrec, 0, r, v );
    setCoordinate( fromCartesian( r[0], r[1], r[2] ) );
}

bool SatellitesItem::initialized()
{
    return true;
}

QString SatellitesItem::itemType() const
{
    return "satellitesitem";
}

bool SatellitesItem::operator<( const Marble::AbstractDataPluginItem *other ) const
{
    return this->id() < other->id();
}

void SatellitesItem::paintViewport( GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    mDebug() << "##########";
    painter->save();

    double r[3], v[3];
    GeoDataLinearRing orbit;
    for ( int i = 1; i < 72; i++ ) {
        sgp4( wgs84, m_satrec, i, r, v );
        orbit << fromCartesian( r[0], r[1], r[2] );
    }

    painter->setPen( oxygenBrickRed4 );
    painter->setBrush( Qt::NoBrush );
    painter->drawPolygon( orbit );

    painter->restore();
}

void SatellitesItem::paint( GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    mDebug() << "Painting " << id();
    double r[3], v[3];
    sgp4( wgs84, m_satrec, 0, r, v );
    //mDebug() << "pos: " << r[0] << " " << r[1] << " " << r[2];
    //mDebug() << "speed: " << v[0] << " " << v[1] << " " << v[2];

    setCoordinate( fromCartesian( r[0], r[1], r[2] ) );

    if ( m_satrec.error != 0 ) {
        mDebug() << "Error: " << m_satrec.error;
        return;
    }

    painter->save();

    painter->setPen( oxygenSkyBlue4 );
    painter->setBrush( oxygenSkyBlue4 );
    painter->drawRect( 0, 0, 15, 15 );

    painter->setPen( Qt::black );
    painter->drawText( 0, 0, id() );

    painter->restore();
}

GeoDataCoordinates SatellitesItem::fromCartesian( double x, double y, double z )
{
    double lat = atan2( y, x );
    double lon = atan2( z, sqrt( x*x + y*y ) );
    double alt = sqrt( x*x + y*y + z*z );
    return GeoDataCoordinates( lat, lon, alt*1000 - EARTH_RADIUS );
}

#include "SatellitesItem.moc"
