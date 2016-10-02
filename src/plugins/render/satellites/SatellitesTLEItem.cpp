//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "SatellitesTLEItem.h"

#include "MarbleClock.h"
#include "MarbleDebug.h"
#include "MarbleGlobal.h"
#include "GeoPainter.h"
#include "GeoDataCoordinates.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataTrack.h"

#include <sgp4ext.h>

#include <QFile>
#include <QDateTime>
#include <QAction>
#include <QColor>

#include <cmath>

namespace Marble {

SatellitesTLEItem::SatellitesTLEItem( const QString &name,
                                      elsetrec satrec,
                                      const MarbleClock *clock )
    : TrackerPluginItem( name ),
      m_satrec( satrec ),
      m_track( new GeoDataTrack() ),
      m_clock( clock )
{
    double tumin, mu, xke, j2, j3, j4, j3oj2;
    double radiusearthkm;
    getgravconst( wgs84, tumin, mu, radiusearthkm, xke, j2, j3, j4, j3oj2 );
    m_earthSemiMajorAxis = radiusearthkm;

    setDescription();

    placemark()->setVisualCategory(GeoDataPlacemark::Satellite);
    placemark()->setZoomLevel( 0 );
    placemark()->setGeometry( m_track );

    update();
}

void SatellitesTLEItem::setDescription()
{
    QFile templateFile(QStringLiteral(":/marble/satellites/satellite.html"));
    if (!templateFile.open(QIODevice::ReadOnly)) {
        placemark()->setDescription(QObject::tr("No info available."));
        return;
    }
    QString html = templateFile.readAll();

    html.replace("%name%", name());
    html.replace("%noradId%", QString::number(m_satrec.satnum));
    html.replace("%perigee%", QString::number(perigee(), 'f', 2));
    html.replace("%apogee%", QString::number(apogee(), 'f', 2));
    html.replace("%inclination%", QString::number(inclination(), 'f', 2));
    html.replace("%period%", QString::number(period(), 'f', 2));
    html.replace("%semiMajorAxis%", QString::number(semiMajorAxis(), 'f', 2));

    placemark()->setDescription( html );
}

void SatellitesTLEItem::update()
{
    if( !isEnabled() ) {
        return;
    }

    QDateTime startTime = m_clock->dateTime();
    QDateTime endTime = startTime;
    if( isTrackVisible() ) {
        startTime = startTime.addSecs( -2 * 60 );
        endTime = startTime.addSecs( period() );
    }

    m_track->removeBefore( startTime );
    m_track->removeAfter( endTime );

    addPointAt( m_clock->dateTime() );

    // time interval between each point in the track, in seconds
    double step = period() / 100.0;

    for ( double i = startTime.toTime_t(); i < endTime.toTime_t(); i += step ) {
        // No need to add points in this interval
        if ( i >= m_track->firstWhen().toTime_t() ) {
            i = m_track->lastWhen().toTime_t() + step;
        }

        addPointAt( QDateTime::fromTime_t( i ) );
    }
}

void SatellitesTLEItem::addPointAt( const QDateTime &dateTime )
{
    // in minutes
    double timeSinceEpoch = (double)( dateTime.toTime_t() -
        timeAtEpoch().toTime_t() ) / 60.0;

    double r[3], v[3];
    sgp4( wgs84, m_satrec, timeSinceEpoch, r, v );

    GeoDataCoordinates coordinates = fromTEME(
        r[0], r[1], r[2], gmst( timeSinceEpoch ) );
    if ( m_satrec.error != 0 ) {
        return;
    }

    m_track->addPoint( dateTime, coordinates);
}

QDateTime SatellitesTLEItem::timeAtEpoch() const
{
    int year = m_satrec.epochyr + ( m_satrec.epochyr < 57 ? 2000 : 1900 );

    int month, day, hours, minutes;
    double seconds;
    days2mdhms( year, m_satrec.epochdays, month, day, hours , minutes, seconds );

    int ms = fmod(seconds * 1000.0, 1000.0);

    return QDateTime( QDate( year, month, day ),
                      QTime( hours, minutes, (int)seconds, ms ),
                      Qt::UTC );
}

double SatellitesTLEItem::period() const
{
    // no := mean motion (rad / min)
    return 60 * (2 * M_PI / m_satrec.no);
}

double SatellitesTLEItem::apogee() const
{
    return m_satrec.alta * m_earthSemiMajorAxis;
}

double SatellitesTLEItem::perigee() const
{
    return m_satrec.altp * m_earthSemiMajorAxis;
}

double SatellitesTLEItem::semiMajorAxis() const
{

    return m_satrec.a * m_earthSemiMajorAxis;
}

double SatellitesTLEItem::inclination() const
{
    return m_satrec.inclo / M_PI * 180;
}

GeoDataCoordinates SatellitesTLEItem::fromTEME( double x,
                                                double y,
                                                double z,
                                                double gmst ) const
{
    double lon = atan2( y, x );
    // Rotate the angle by gmst (the origin goes from the vernal equinox
    // point to the Greenwich Meridian)
    lon = GeoDataCoordinates::normalizeLon( fmod(lon - gmst, 2 * M_PI) );

    double lat = atan2( z, sqrt( x*x + y*y ) );

    //TODO: determine if this is worth the extra precision
    // Algorithm from http://celestrak.com/columns/v02n03/
    //TODO: demonstrate it.
    double a = m_earthSemiMajorAxis;
    double planetRadius = sqrt( x*x + y*y );
    double latp = lat;
    double C;
    for ( int i = 0; i < 3; i++ ) {
        C = 1 / sqrt( 1 - square( m_satrec.ecco * sin( latp ) ) );
        lat = atan2( z + a * C * square( m_satrec.ecco ) * sin( latp ), planetRadius );
    }

    double alt = planetRadius / cos( lat ) - a * C;

    lat = GeoDataCoordinates::normalizeLat( lat );

    return GeoDataCoordinates( lon, lat, alt * 1000 );
}

double SatellitesTLEItem::gmst( double minutesP ) const
{
    // Earth rotation rate in rad/min, from sgp4io.cpp
    double rptim = 4.37526908801129966e-3;
    return fmod( m_satrec.gsto + rptim * minutesP, 2 * M_PI );
}

double SatellitesTLEItem::square( double x )
{
    return x * x;
}

} // namespace Marble
