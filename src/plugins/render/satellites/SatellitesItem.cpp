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

#include "sgp4/sgp4ext.h"

#include <QtCore/QDateTime>

#include <cmath>

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
    sgp4( wgs84, m_satrec, timeSinceEpoch(), r, v );
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

    painter->save();

    double r[3], v[3];
    GeoDataLinearRing orbit;
    int startTime = timeSinceEpoch();
    int endTime = startTime + orbitalPeriod() / 60 + 1;
    for ( int i = startTime; i < endTime; i++ ) {
        sgp4( wgs84, m_satrec, i, r, v );
        mDebug() << i;
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

    mDebug() << "Painting " << id() << orbitalPeriod();
    double r[3], v[3];
    sgp4( wgs84, m_satrec, timeSinceEpoch(), r, v );
    //mDebug() << "pos: " << r[0] << " " << r[1] << " " << r[2];
    //mDebug() << "speed: " << v[0] << " " << v[1] << " " << v[2];

    //FIXME: this should be done in a function that is guaranteed to be called regularly
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

// Hopefully this is correct enough
double SatellitesItem::timeSinceEpoch()
{
    int year = m_satrec.epochyr + ( m_satrec.epochyr < 57 ? 2000 : 1900 );

    int month, day, hours, minutes;
    double seconds;
    days2mdhms( year, m_satrec.epochdays, month, day, hours , minutes, seconds );
    QDateTime time = QDateTime( QDate( year, month, day ),
                                QTime( hours, minutes, (int)seconds, (int)( seconds / 1000.0 ) ) );
    return (double)( QDateTime::currentMSecsSinceEpoch() - time.toMSecsSinceEpoch() ) / ( 1000.0 * 60.0 );
}

double SatellitesItem::orbitalPeriod()
{
    double r[3], v[3];
    sgp4( wgs84, m_satrec, timeSinceEpoch(), r, v );
    double radiusearthkm, tumin, xke, j2, j3, j4, j3oj2;
    double mu; // gravitational parameter (km^3 / s^2)
    getgravconst( wgs84, tumin, mu, radiusearthkm, xke, j2, j3, j4, j3oj2 );
    double p, ecc, incl, omega, argp, nu, m, arglat, truelon, lonper;
    double a; // semi-major axis (km)
    rv2coe( r, v, mu, p, a, ecc, incl, omega, argp, nu, m, arglat, truelon, lonper);
    return 2 * M_PI * a * sqrt ( a / mu );
}

GeoDataCoordinates SatellitesItem::fromCartesian( double x, double y, double z )
{
    double lat = atan2( y, x );
    double lon = atan2( z, sqrt( x*x + y*y ) );
    double alt = sqrt( x*x + y*y + z*z );
    return GeoDataCoordinates( lat, lon, alt*1000 - EARTH_RADIUS );
}

#include "SatellitesItem.moc"
