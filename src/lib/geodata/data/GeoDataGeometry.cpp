//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn <rahn@kde.org>
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>"
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
    return d_geom->m_extrude;
}

void GeoDataGeometry::setExtrude( bool extrude )
{
    d_geom->m_extrude = extrude;
}

bool GeoDataGeometry::tessellate() const
{
    return d_geom->m_tessellate;
}

void GeoDataGeometry::setTessellate( bool tessellate )
{
    d_geom->m_tessellate = tessellate;
}

AltitudeMode GeoDataGeometry::altitudeMode() const
{
    return d_geom->m_altitudeMode;
}

void GeoDataGeometry::setAltitudeMode( const AltitudeMode altitudeMode )
{
    d_geom->m_altitudeMode = altitudeMode;
}


GeoDataGeometry::GeoDataGeometry() : d_geom( new GeoDataGeometryPrivate() )
{
}

GeoDataGeometry::GeoDataGeometry( const GeoDataGeometry& other )
    : d_geom( new GeoDataGeometryPrivate() )
{
    *d_geom = *other.d_geom;
}

GeoDataGeometry& GeoDataGeometry::operator=( const GeoDataGeometry& other )
{
    *d_geom = *other.d_geom;
    return *this;
}

GeoDataGeometry::~GeoDataGeometry()
{
    delete d_geom;
}
