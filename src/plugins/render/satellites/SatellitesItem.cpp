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
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"

#include "sgp4/sgp4ext.h"

#include <QtCore/QDateTime>
#include <QtGui/QAction>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

#include <cmath>
#include <QDialog>
#include <QCheckBox>

using namespace Marble;

#include "GeoDataPoint.h"

SatellitesItem::SatellitesItem( const QString &name, elsetrec satrec )
    : TrackerPluginItem( name ),
      m_showOrbit( false ),
      m_satrec( satrec ),
      m_orbit( new GeoDataLineString() )
{
    double tumin, mu, xke, j2, j3, j4, j3oj2;
    double radiusearthkm;
    getgravconst( wgs84, tumin, mu, radiusearthkm, xke, j2, j3, j4, j3oj2 );
    m_earthSemiMajorAxis = radiusearthkm;

    setDescription();

    placemark()->setVisualCategory( GeoDataFeature::Satellite );

    m_point = new GeoDataPoint( *static_cast<GeoDataPoint *>( placemark()->geometry() ) );
    GeoDataMultiGeometry *multiGeometry = new GeoDataMultiGeometry();
    multiGeometry->append( m_point );
    multiGeometry->append( m_orbit );
    placemark()->setGeometry( multiGeometry );

    GeoDataStyle *style = new GeoDataStyle( *placemark()->style() );
    placemark()->setStyle( style );
    placemark()->style()->lineStyle().setColor( oxygenBrickRed4 );
    placemark()->style()->lineStyle().setPenStyle( Qt::NoPen );
}

void SatellitesItem::setDescription()
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
              QString::number( period() ), QString::number( semiMajorAxis() ) );
     placemark()->setDescription( description );
}

void SatellitesItem::update()
{
    double r[3], v[3];
    double t = timeSinceEpoch();
    sgp4( wgs84, m_satrec, t, r, v );

    if ( m_satrec.error != 0 ) {
        mDebug() << "Error: " << m_satrec.error;
        return;
    }

    double lon, lat, alt;
    fromTEME( r[0], r[1], r[2], gmst( t ) ).geoCoordinates( lon, lat, alt );
    m_point->set( lon, lat, alt );

    if ( placemark()->style()->lineStyle().penStyle() != Qt::NoPen ) {
        m_orbit->clear();
        double startTime = timeSinceEpoch();
        double endTime = startTime + period() + 1;
        for ( int i = startTime; i < endTime; i++ ) {
            sgp4( wgs84, m_satrec, i, r, v );
            *m_orbit << fromTEME( r[0], r[1], r[2], gmst( i ) );
        }
    }
}

// Hopefully this is correct enough
double SatellitesItem::timeSinceEpoch()
{
    int year = m_satrec.epochyr + ( m_satrec.epochyr < 57 ? 2000 : 1900 );

    int month, day, hours, minutes;
    double seconds;
    days2mdhms( year, m_satrec.epochdays, month, day, hours , minutes, seconds );
    QDateTime time = QDateTime( QDate( year, month, day ),
                                QTime( hours, minutes, (int)seconds, (int)( seconds / 1000.0 ) ),
                                Qt::UTC );
    //TODO: use MarbleClock
    #if QT_VERSION < 0x040700
    qint64 currentTimestamp = QDateTime::currentDateTime().toTime_t() * 1000;
    qint64 epochTimestamp = time.toTime_t() * 1000;
    #else
    qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();
    qint64 epochTimestamp = time.toMSecsSinceEpoch();
    #endif
    return (double)( currentTimestamp - epochTimestamp ) / ( 1000.0 * 60.0 );
}

double SatellitesItem::period()
{
    // no := mean motion (rad / min)
    double T = 1 / m_satrec.no;
    return T * 2 * M_PI;
}

double SatellitesItem::apogee()
{
    return m_satrec.alta * m_earthSemiMajorAxis;
}

double SatellitesItem::perigee()
{
    return m_satrec.altp * m_earthSemiMajorAxis;
}

double SatellitesItem::semiMajorAxis()
{

    return m_satrec.a * m_earthSemiMajorAxis;
}

double SatellitesItem::inclination()
{
    return m_satrec.inclo / M_PI * 180;
}

GeoDataCoordinates SatellitesItem::fromTEME( double x, double y, double z, double gmst )
{
    double lon = atan2( y, x );
    // Rotate the angle by gmst (the origin goes from the vernal equinox point to the Greenwich Meridian)
    lon = fmod(lon - gmst, 2 * M_PI);

    double lat = atan2( z, sqrt( x*x + y*y ) );

    //TODO: determine if the extra precision is worth it.
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

    return GeoDataCoordinates( lon, lat, alt * 1000 );
}

double SatellitesItem::gmst( double minutesP )
{
    // Earth rotation rate in rad/min, from sgp4io.cpp
    double rptim = 4.37526908801129966e-3;
    return fmod( m_satrec.gsto + rptim * minutesP, 2 * M_PI );
}

double SatellitesItem::square( double x )
{
    return x * x;
}

#include "SatellitesItem.moc"
