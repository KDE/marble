//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include <QtCore/QDataStream>
#include <QtCore/QDebug>

#include "GeoDataPlacemark.h"


GeoDataPlacemark::GeoDataPlacemark() :
    m_area( -1.0 ),
    m_population( -1 )
{
}

GeoDataPlacemark::GeoDataPlacemark( const QString& name )
  : GeoDataFeature( name ),
    m_area( -1.0 ),
    m_population( -1 )
{
}

GeoPoint GeoDataPlacemark::coordinate() const
{
    return m_coordinate;
}

void GeoDataPlacemark::coordinate( double& lon, double& lat, double& alt )
{
    m_coordinate.geoCoordinates( lon, lat );
    alt = m_coordinate.altitude();
}

void GeoDataPlacemark::setCoordinate( double lon, double lat, double alt )
{
    m_coordinate = GeoPoint( lon, lat, alt );
}

const double GeoDataPlacemark::area() const
{
    return m_area;
}

void GeoDataPlacemark::setArea( double area )
{
    m_area = area;
}

const qint64 GeoDataPlacemark::population() const
{
    return m_population;
}

void GeoDataPlacemark::setPopulation( qint64 population )
{
    m_population = population;
}

const QString GeoDataPlacemark::countryCode() const
{
    return m_countrycode;
}

void GeoDataPlacemark::setCountryCode( const QString &countrycode )
{
    m_countrycode = countrycode;
}

void GeoDataPlacemark::pack( QDataStream& stream ) const
{
    GeoDataFeature::pack( stream );

    stream << m_countrycode;

    /*
     * pack coordinates
     */
    double longitude;
    double latitude;

    m_coordinate.geoCoordinates( longitude, latitude );
    stream << longitude;
    stream << latitude;
}


void GeoDataPlacemark::unpack( QDataStream& stream )
{
    GeoDataFeature::unpack( stream );

    stream >> m_area;
    stream >> m_population;
    stream >> m_countrycode;

    /*
     * unpack coordinates
     */
    double longitude;
    double latitude;

    stream >> longitude;
    stream >> latitude;
    setCoordinate( longitude, latitude );
}
