//
// This file is part of the Marble Desktop Globe.
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

#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataLineString.h"
#include "GeoDataMultiGeometry.h"

#include <QtCore/QDebug>

#include "GeoDataGeometry_p.h"

namespace Marble
{

GeoDataGeometry::GeoDataGeometry( GeoDataObject *parent ) 
    : GeoDataObject( parent ),
      d( new GeoDataGeometryPrivate() )
{
    p()->ref.ref();
}

GeoDataGeometry::GeoDataGeometry( const GeoDataGeometry& other )
    : GeoDataObject(),
      d( other.d )
{
    p()->ref.ref();
}

GeoDataGeometry::GeoDataGeometry( const GeoDataPoint& other )
    : GeoDataObject(),
      d( other.GeoDataGeometry::d )
{
    p()->ref.ref();
}

GeoDataGeometry::GeoDataGeometry( const GeoDataPolygon& other )
    : GeoDataObject(),
      d( other.GeoDataGeometry::d )
{
    p()->ref.ref();
}

GeoDataGeometry::GeoDataGeometry( const GeoDataLineString& other )
    : GeoDataObject(),
      d( other.GeoDataGeometry::d )
{
    p()->ref.ref();
}

GeoDataGeometry::GeoDataGeometry( const GeoDataMultiGeometry& other )
    : GeoDataObject(),
      d( other.GeoDataGeometry::d )
{
    p()->ref.ref();
}

GeoDataGeometry::GeoDataGeometry( GeoDataGeometryPrivate* priv )
    : GeoDataObject(),
      d( priv )
{
    p()->ref.ref();
}

GeoDataGeometry::~GeoDataGeometry()
{
    if (!p()->ref.deref())
        delete d;
}

void  GeoDataGeometry::detach()
{
    if(p()->ref == 1)
        return;

     GeoDataGeometryPrivate* new_d = static_cast< GeoDataGeometryPrivate*>(p()->copy());

    if (!p()->ref.deref())
        delete d;

    d = new_d;
}

GeoDataGeometryPrivate* GeoDataGeometry::p() const
{
    return static_cast<GeoDataGeometryPrivate*>(d);
}

GeoDataGeometry& GeoDataGeometry::operator=( const GeoDataGeometry& other )
{
    if (!p()->ref.deref())
        delete d;

    d = other.d;
    p()->ref.ref();
    
    return *this;
}

bool GeoDataGeometry::extrude() const
{
    return p()->m_extrude;
}

void GeoDataGeometry::setExtrude( bool extrude )
{
    p()->m_extrude = extrude;
}

AltitudeMode GeoDataGeometry::altitudeMode() const
{
    return p()->m_altitudeMode;
}

void GeoDataGeometry::setAltitudeMode( const AltitudeMode altitudeMode )
{
    p()->m_altitudeMode = altitudeMode;
}

void GeoDataGeometry::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << p()->m_extrude;
    stream << p()->m_altitudeMode;
}

void GeoDataGeometry::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    int am;
    stream >> p()->m_extrude;
    stream >> am;
    p()->m_altitudeMode = (AltitudeMode) am;
}

}
