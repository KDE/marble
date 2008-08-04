//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn <rahn@kde.org>
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>"
// Copyright 2008      Inge Wallin <inge@lysator.liu.se>"
//


#include "GeoDataGeometry.h"

#include <QtCore/QDebug>

class GeoDataGeometryPrivate
{
 public:
    GeoDataGeometryPrivate()
        : m_extrude( false ),
          m_tessellate( false ),
          m_altitudeMode( ClampToGround )
    {
    }

    bool         m_extrude;
    bool         m_tessellate;
    AltitudeMode m_altitudeMode;
};

bool GeoDataGeometry::extrude() const
{
    return d->m_extrude;
}

void GeoDataGeometry::setExtrude( bool extrude )
{
    d->m_extrude = extrude;
}

bool GeoDataGeometry::tessellate() const
{
    return d->m_tessellate;
}

void GeoDataGeometry::setTessellate( bool tessellate )
{
    d->m_tessellate = tessellate;
}

AltitudeMode GeoDataGeometry::altitudeMode() const
{
    return d->m_altitudeMode;
}

void GeoDataGeometry::setAltitudeMode( const AltitudeMode altitudeMode )
{
    d->m_altitudeMode = altitudeMode;
}


GeoDataGeometry::GeoDataGeometry() 
    : d( new GeoDataGeometryPrivate() )
{
}

GeoDataGeometry::GeoDataGeometry( const GeoDataGeometry& other )
    : GeoDataObject(),
      d( new GeoDataGeometryPrivate() )
{
    *d = *other.d;
}

GeoDataGeometry& GeoDataGeometry::operator=( const GeoDataGeometry& other )
{
    *d = *other.d;
    return *this;
}

GeoDataGeometry::~GeoDataGeometry()
{
    delete d;
}

void GeoDataGeometry::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << d->m_extrude;
    stream << d->m_tessellate;
    stream << d->m_altitudeMode;
}

void GeoDataGeometry::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    int am;
    stream >> d->m_extrude;
    stream >> d->m_tessellate;
    stream >> am;
    d->m_altitudeMode = (AltitudeMode) am;
}
