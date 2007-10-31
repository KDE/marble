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

#include "GeoDataPlaceMark.h"


GeoDataPlaceMark::GeoDataPlaceMark()
{
}

GeoDataPlaceMark::GeoDataPlaceMark( const QString& name )
  : GeoDataFeature( name )
{
}

GeoPoint GeoDataPlaceMark::coordinate() const
{
    return m_coordinate;
}

void GeoDataPlaceMark::coordinate( double& lon, double& lat )
{
    m_coordinate.geoCoordinates( lon, lat );
}

void GeoDataPlaceMark::setCoordinate( double lon, double lat )
{
    m_coordinate = GeoPoint( lon, lat );
}

const QString GeoDataPlaceMark::countryCode() const
{
    return m_countrycode;
}

void GeoDataPlaceMark::setCountryCode( const QString &countrycode )
{
    m_countrycode = countrycode;
}

void GeoDataPlaceMark::pack( QDataStream& stream ) const
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


void GeoDataPlaceMark::unpack( QDataStream& stream )
{
    GeoDataFeature::unpack( stream );

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
