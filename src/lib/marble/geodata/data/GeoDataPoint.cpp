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
    Q_D(GeoDataPoint);
    d->m_coordinates = GeoDataCoordinates(lon, lat, alt, unit);
    d->m_latLonAltBox = GeoDataLatLonAltBox(d->m_coordinates);
}

GeoDataPoint::GeoDataPoint( const GeoDataPoint& other )
    : GeoDataGeometry( other )
{
    Q_D(GeoDataPoint);
    const GeoDataPointPrivate * const otherD = other.d_func();

    d->m_coordinates = otherD->m_coordinates;
    d->m_latLonAltBox = otherD->m_latLonAltBox;
}

GeoDataPoint::GeoDataPoint( const GeoDataCoordinates& other )
    : GeoDataGeometry ( new GeoDataPointPrivate )
{
    Q_D(GeoDataPoint);
    d->m_coordinates = other;
    d->m_latLonAltBox = GeoDataLatLonAltBox(d->m_coordinates);
}

GeoDataPoint::GeoDataPoint()
    : GeoDataGeometry( new GeoDataPointPrivate )
{
    // nothing to do
}

GeoDataPoint::~GeoDataPoint()
{
    // nothing to do
}

bool GeoDataPoint::operator==( const GeoDataPoint &other ) const
{
    return equals(other) &&
           coordinates() == other.coordinates();
}

bool GeoDataPoint::operator!=( const GeoDataPoint &other ) const
{
    return !this->operator==(other);
}

void GeoDataPoint::setCoordinates( const GeoDataCoordinates &coordinates )
{
    detach();

    Q_D(GeoDataPoint);
    d->m_coordinates = coordinates;
    d->m_latLonAltBox = GeoDataLatLonAltBox(d->m_coordinates);
}

const GeoDataCoordinates &GeoDataPoint::coordinates() const
{
    Q_D(const GeoDataPoint);
    return d->m_coordinates;
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
    Q_D(const GeoDataPoint);
    d->m_coordinates.pack(stream);
    // TODO: what about m_latLonAltBox and base class?
}

void GeoDataPoint::unpack( QDataStream& stream )
{
    Q_D(GeoDataPoint);
    d->m_coordinates.unpack(stream);
    // TODO: what about m_latLonAltBox and base class?
}

}
