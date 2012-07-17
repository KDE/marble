//
// This file is part of the Marble Virtual Globe.
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
#include "GeoDataPoint_p.h"
#include "GeoDataCoordinates.h"

#include <cmath>

#include <QtCore/QCoreApplication>

#include "MarbleDebug.h"
#include "MarbleGlobal.h"

#include "GeoDataTypes.h"
#include "GeoDataLatLonAltBox.h"


namespace Marble
{

GeoDataPoint::GeoDataPoint( qreal _lon, qreal _lat, qreal _alt,
                            GeoDataPoint::Unit unit, int _detail )
  : GeoDataCoordinates( _lon, _lat, _alt, 
                        static_cast<GeoDataCoordinates::Unit>( unit ), _detail ),
    GeoDataGeometry( new GeoDataPointPrivate )
{
    p()->m_latLonAltBox = *this;
}

GeoDataPoint::GeoDataPoint( const GeoDataPoint& other )
  : GeoDataCoordinates( other ),
    GeoDataGeometry( other )
    
{
    p()->m_latLonAltBox = *this;
}

GeoDataPoint::GeoDataPoint( const GeoDataCoordinates& other )
  : GeoDataCoordinates( other ),
  GeoDataGeometry ( new GeoDataPointPrivate )
{
    p()->m_latLonAltBox = *this;
}

GeoDataPoint::GeoDataPoint( const GeoDataGeometry& other )
  : GeoDataGeometry( other )

{
}

GeoDataPoint::GeoDataPoint()
    : GeoDataGeometry( new GeoDataPointPrivate )
{
}

GeoDataPoint::~GeoDataPoint()
{
}

GeoDataPointPrivate* GeoDataPoint::p() const
{
    return static_cast<GeoDataPointPrivate*>(GeoDataGeometry::d);
}

const char* GeoDataPoint::nodeType() const
{
    return GeoDataTypes::GeoDataPointType;
}

void GeoDataPoint::detach()
{
    GeoDataCoordinates::detach();
    GeoDataGeometry::detach();
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
