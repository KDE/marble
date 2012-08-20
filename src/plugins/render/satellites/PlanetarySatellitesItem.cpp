//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//


#include "PlanetarySatellitesItem.h"

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

PlanetarySatellitesItem::PlanetarySatellitesItem( const QString &name, 
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

QString PlanetarySatellitesItem::name() const
{
    return m_name;
}

void PlanetarySatellitesItem::setDescription()
{
    QString description =
      QObject::tr( "Object name: %1 <br />"
                   "Pericentre: %2 <br />"
                   "Apocentre: %3 <br />"
                   "Inclination: %4 <br />"
                   "Revolutions per day (24h): %5" )
        .arg( name(), QString::number( m_perc ), QString::number( m_apoc ),
              QString::number( m_inc ), QString::number( m_n0 ) );
     placemark()->setDescription( description );
}

void PlanetarySatellitesItem::update()
{
    double lng    = 0.;
    double lat    = 0.;
    double height = 0.;

    m_planSat->getTime();
    m_planSat->currentPos();
    m_planSat->getPlanetographic( lng, lat, height );

    qDebug() << "Update" << m_name << lng << lat << height;

    GeoDataCoordinates coords( lng, lat, height * 1000, GeoDataCoordinates::Degree );
    m_track->addPoint( m_clock->dateTime(), coords );
}

} // namespace Marble

