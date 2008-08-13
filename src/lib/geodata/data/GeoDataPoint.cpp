//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataPoint.h"
#include "GeoDataCoordinates.h"

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include "global.h"

namespace Marble
{

GeoDataPoint::GeoDataPoint( double _lon, double _lat, double _alt, 
                            GeoDataPoint::Unit unit, int _detail )
  : GeoDataCoordinates( _lon, _lat, _alt, 
                        static_cast<GeoDataCoordinates::Unit>( unit ), _detail )
{
}

GeoDataPoint::GeoDataPoint( const GeoDataPoint& other )
  : GeoDataCoordinates( other ),
    GeoDataGeometry( other )
{
}

GeoDataPoint::GeoDataPoint()
{
}

GeoDataPoint::~GeoDataPoint()
{
}

GeoDataPoint& GeoDataPoint::operator=( const GeoDataPoint &other )
{
    GeoDataCoordinates::operator=( other );
    return *this;
}

void GeoDataPoint::pack( QDataStream& stream ) const
{
    GeoDataCoordinates::pack( stream );
}

void GeoDataPoint::unpack( QDataStream& stream )
{
    GeoDataCoordinates::unpack( stream );
}

}
