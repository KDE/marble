//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "GeoDataPhotoOverlay.h"
#include "GeoDataTypes.h"

namespace Marble {

class GeoDataPhotoOverlayPrivate
{
public:
    GeoDataPoint m_point;
};

GeoDataPhotoOverlay::GeoDataPhotoOverlay() : d( new GeoDataPhotoOverlayPrivate )
{
    // nothing to do
}

GeoDataPhotoOverlay::GeoDataPhotoOverlay( const Marble::GeoDataPhotoOverlay &other ) :
    GeoDataOverlay( other ), d( new GeoDataPhotoOverlayPrivate( *other.d ) )
{
    // nothing to do
}

GeoDataPhotoOverlay &GeoDataPhotoOverlay::operator=( const GeoDataPhotoOverlay &other )
{
    *d = *other.d;
    return *this;
}

GeoDataPhotoOverlay::~GeoDataPhotoOverlay()
{
    delete d;
}

const char *GeoDataPhotoOverlay::nodeType() const
{
    return GeoDataTypes::GeoDataPhotoOverlayType;
}

GeoDataPoint &GeoDataPhotoOverlay::point() const
{
    return d->m_point;
}

void GeoDataPhotoOverlay::setPoint( const GeoDataPoint &point )
{
    d->m_point = point;
}

}
