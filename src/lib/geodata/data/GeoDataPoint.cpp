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

GeoDataPoint::GeoDataPoint( qreal lon, qreal lat, qreal alt,
                            GeoDataCoordinates::Unit unit )
  : GeoDataGeometry( new GeoDataPointPrivate )
{
    p()->m_coordinates = GeoDataCoordinates( lon, lat, alt, unit );
    p()->m_latLonAltBox = GeoDataLatLonAltBox( p()->m_coordinates );
}

GeoDataPoint::GeoDataPoint( const GeoDataPoint& other )
  : GeoDataGeometry( other )
    
{
    p()->m_coordinates = other.p()->m_coordinates;
    p()->m_latLonAltBox = other.p()->m_latLonAltBox;
}

GeoDataPoint::GeoDataPoint( const GeoDataCoordinates& other )
  : GeoDataGeometry ( new GeoDataPointPrivate )
{
    p()->m_coordinates = other;
    p()->m_latLonAltBox = GeoDataLatLonAltBox( p()->m_coordinates );
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

void GeoDataPoint::setCoordinates( const GeoDataCoordinates &coordinates )
{
    p()->m_coordinates = coordinates;
    p()->m_latLonAltBox = GeoDataLatLonAltBox( p()->m_coordinates );
}

const GeoDataCoordinates &GeoDataPoint::coordinates() const
{
    return p()->m_coordinates;
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
    GeoDataGeometry::detach();
}

void GeoDataPoint::pack( QDataStream& stream ) const
{
    p()->m_coordinates.pack( stream );
}

void GeoDataPoint::unpack( QDataStream& stream )
{
    p()->m_coordinates.unpack( stream );
}

}
