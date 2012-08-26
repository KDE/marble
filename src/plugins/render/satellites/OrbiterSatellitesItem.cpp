//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//


#include "OrbiterSatellitesItem.h"

#include <QtCore>

#include "MarbleClock.h"
#include "MarbleDebug.h"
#include "MarbleGlobal.h"
#include "GeoPainter.h"
#include "GeoDataCoordinates.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataTrack.h"
#include "GeoDataPoint.h"

namespace Marble {

OrbiterSatellitesItem::OrbiterSatellitesItem( const QString &name, 
                                              PlanetarySats *planSat,
                                              const MarbleClock *clock )
    : TrackerPluginItem( name ),
      m_showOrbit( false ),
      m_track( new GeoDataTrack() ),
      m_clock( clock ),
      m_planSat( planSat ),
      m_name( name )
{
    placemark()->setVisualCategory( GeoDataFeature::Satellite );
    placemark()->setZoomLevel( 0 );
    placemark()->setGeometry( m_track );

    GeoDataStyle *style = new GeoDataStyle( *placemark()->style() );
    placemark()->setStyle( style );
    placemark()->style()->lineStyle().setColor( oxygenBrickRed4 );
    placemark()->style()->lineStyle().setPenStyle( Qt::NoPen );
    placemark()->style()->labelStyle().setGlow( true );

    m_planSat->getKeplerElements(
        m_perc, m_apoc, m_inc, m_ecc, m_ra, m_tano, m_m0, m_a, m_n0 );

    setDescription();
    update();
}

OrbiterSatellitesItem::~OrbiterSatellitesItem()
{
    delete m_planSat;
}

QString OrbiterSatellitesItem::name() const
{
    return m_name;
}

void OrbiterSatellitesItem::setDescription()
{
    QString description =
      QObject::tr( "Object name: %1 <br />"
                   "Pericentre: %2 km<br />"
                   "Apocentre: %3 km<br />"
                   "Inclination: %4 Degree<br />"
                   "Revolutions per day (24h): %5" )
        .arg( name(), QString::number( m_perc, 'f', 2 ),
                      QString::number( m_apoc, 'f', 2 ),
                      QString::number( m_inc, 'f', 2 ),
                      QString::number( m_n0, 'f', 2 ) );
     placemark()->setDescription( description );
}

void OrbiterSatellitesItem::update()
{
    double period = 24  * 3600 / m_n0;
    QDateTime startTime = m_clock->dateTime().addSecs( - period / 2. );
    QDateTime endTime = startTime.addSecs( period );

    m_track->removeBefore( startTime );
    m_track->removeAfter( endTime );

    double step = period / 100.;

    // FIXME update track only if orbit is visible
    for( double i = startTime.toTime_t(); i < endTime.toTime_t(); i += step ) {

        if ( i >= m_track->firstWhen().toTime_t() ) {
            i = m_track->lastWhen().toTime_t() + step;
        }

        addTrackPointAt( QDateTime::fromTime_t( i ) );
    }

    addTrackPointAt( m_clock->dateTime() );
}

void OrbiterSatellitesItem::addTrackPointAt( const QDateTime &dateTime )
{
    double lng    = 0.;
    double lat    = 0.;
    double height = 0.;

    QDateTime dt = dateTime.toUTC();
    QDate date = dt.date();
    QTime time = dt.time();

    m_planSat->setMJD( date.year(), date.month(), date.day(),
                       time.hour(), time.minute(), time.second() );
    m_planSat->currentPos();
    m_planSat->getPlanetographic( lng, lat, height );

    m_track->addPoint( dateTime,
                       GeoDataCoordinates( lng, lat, height * 1000,
                                           GeoDataCoordinates::Degree) );
}

} // namespace Marble

