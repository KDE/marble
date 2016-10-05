//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn <rahn@kde.org>
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2008      Inge Wallin <inge@lysator.liu.se>
//


#include "GeoDataGeometry.h"
#include "GeoDataGeometry_p.h"

#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataLineString.h"
#include "GeoDataMultiGeometry.h"

#include "MarbleDebug.h"

#include <QDataStream>


namespace Marble
{

GeoDataGeometry::GeoDataGeometry()
    : d_ptr(new GeoDataGeometryPrivate())
{
    d_ptr->ref.ref();
}

GeoDataGeometry::GeoDataGeometry( const GeoDataGeometry& other )
    : GeoDataObject(),
      d_ptr(other.d_ptr)
{
    d_ptr->ref.ref();
}

GeoDataGeometry::GeoDataGeometry( GeoDataGeometryPrivate* priv )
    : GeoDataObject(),
      d_ptr(priv)
{
    d_ptr->ref.ref();
}

GeoDataGeometry::~GeoDataGeometry()
{
    if (!d_ptr->ref.deref())
        delete d_ptr;
}

void GeoDataGeometry::detach()
{
    if(d_ptr->ref.load() == 1) {
        return;
    }

     GeoDataGeometryPrivate* new_d = d_ptr->copy();

    if (!d_ptr->ref.deref())
        delete d_ptr;

    d_ptr = new_d;
    d_ptr->ref.ref();
}

const char* GeoDataGeometry::nodeType() const
{
    return d_ptr->nodeType();
}

EnumGeometryId GeoDataGeometry::geometryId() const
{
    return d_ptr->geometryId();
}

GeoDataGeometry& GeoDataGeometry::operator=( const GeoDataGeometry& other )
{
    GeoDataObject::operator=( other );

    if (!d_ptr->ref.deref())
        delete d_ptr;

    d_ptr = other.d_ptr;
    d_ptr->ref.ref();
    
    return *this;
}

bool GeoDataGeometry::extrude() const
{
    return d_ptr->m_extrude;
}

void GeoDataGeometry::setExtrude( bool extrude )
{
    detach();
    d_ptr->m_extrude = extrude;
}

AltitudeMode GeoDataGeometry::altitudeMode() const
{
    return d_ptr->m_altitudeMode;
}

void GeoDataGeometry::setAltitudeMode( const AltitudeMode altitudeMode )
{
    detach();
    d_ptr->m_altitudeMode = altitudeMode;
}

const GeoDataLatLonAltBox& GeoDataGeometry::latLonAltBox() const
{
    return d_ptr->m_latLonAltBox;
}

void GeoDataGeometry::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << d_ptr->m_extrude;
    stream << d_ptr->m_altitudeMode;
}

void GeoDataGeometry::unpack( QDataStream& stream )
{
    detach();
    GeoDataObject::unpack( stream );

    int am;
    stream >> d_ptr->m_extrude;
    stream >> am;
    d_ptr->m_altitudeMode = (AltitudeMode) am;
}

bool GeoDataGeometry::equals(const GeoDataGeometry &other) const
{
    return GeoDataObject::equals(other) &&
           d_ptr->m_extrude == other.d_ptr->m_extrude &&
           d_ptr->m_altitudeMode == other.d_ptr->m_altitudeMode;
}

}
