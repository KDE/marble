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

#include "sgp4/sgp4ext.h"

#include <QtCore/QDateTime>
#include <QtGui/QAction>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

#include <cmath>
#include <QDialog>
#include <QCheckBox>

namespace Marble {

#include "GeoDataPoint.h"

SatellitesTLEItem::SatellitesTLEItem( const QString &name,
                                      elsetrec satrec,
                                      const MarbleClock *clock )
    : TrackerPluginItem( name ),
      m_showOrbit( false ),
      m_satrec( satrec ),
      m_track( new GeoDataTrack() ),
      m_clock( clock )
{
    double tumin, mu, xke, j2, j3, j4, j3oj2;
    double radiusearthkm;
    getgravconst( wgs84, tumin, mu, radiusearthkm, xke, j2, j3, j4, j3oj2 );
    m_earthSemiMajorAxis = radiusearthkm;

    setDescription();

    placemark()->setVisualCategory( GeoDataFeature::Satellite );
    placemark()->setZoomLevel( 0 );
    placemark()->setGeometry( m_track );

    GeoDataStyle *style = new GeoDataStyle( *placemark()->style() );
    placemark()->setStyle( style );
    placemark()->style()->lineStyle().setColor( oxygenBrickRed4 );
    placemark()->style()->lineStyle().setPenStyle( Qt::NoPen );
    placemark()->style()->labelStyle().setGlow( true );

    update();
}

void SatellitesTLEItem::setDescription()
{
    QString description =
      QObject::tr( "NORAD ID: %2 <br />"
                   "Perigee: %3 km <br />"
                   "Apogee: %4 km <br />"
                   "Inclination: %5 degrees <br />"
                   "Period: %6 minutes <br />"
                   "Semi-major axis: %7 km" )
        .arg( QString::number( m_satrec.satnum ), QString::number( perigee() ),
              QString::number( apogee() ), QString::number( inclination() ),
              QString::number( period() / 60.0 ),
              QString::number( semiMajorAxis() ) );
     placemark()->setDescription( description );
}

void SatellitesTLEItem::update()
{
    QDateTime startTime = m_clock->dateTime().addSecs( - 2 * 60 );

    QDateTime endTime = startTime.addSecs( period() );

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

QDateTime SatellitesTLEItem::timeAtEpoch()
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

double SatellitesTLEItem::period()
{
    // no := mean motion (rad / min)
    return 60 * (2 * M_PI / m_satrec.no);
}

double SatellitesTLEItem::apogee()
{
    return m_satrec.alta * m_earthSemiMajorAxis;
}

double SatellitesTLEItem::perigee()
{
    return m_satrec.altp * m_earthSemiMajorAxis;
}

double SatellitesTLEItem::semiMajorAxis()
{

    return m_satrec.a * m_earthSemiMajorAxis;
}

double SatellitesTLEItem::inclination()
{
    return m_satrec.inclo / M_PI * 180;
}

GeoDataCoordinates SatellitesTLEItem::fromTEME( double x,
                                               double y,
                                               double z,
                                               double gmst )
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
    double R = sqrt( x*x + y*y );
    double latp = lat;
    double C;
    for ( int i = 0; i < 3; i++ ) {
        C = 1 / sqrt( 1 - square( m_satrec.ecco * sin( latp ) ) );
        lat = atan2( z + a * C * square( m_satrec.ecco ) * sin( latp ), R );
    }

    double alt = R / cos( lat ) - a * C;

    lat = GeoDataCoordinates::normalizeLat( lat );

    return GeoDataCoordinates( lon, lat, alt * 1000 );
}

double SatellitesTLEItem::gmst( double minutesP )
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

#include "SatellitesTLEItem.moc"

