//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "GeoDataGroundOverlay.h"
#include "GeoDataTypes.h"

namespace Marble {

class GeoDataGroundOverlayPrivate
{
public:
    double m_altitude;

    AltitudeMode m_altitudeMode;

    GeoDataLatLonBox m_latLonBox;

    GeoDataGroundOverlayPrivate();
};

GeoDataGroundOverlayPrivate::GeoDataGroundOverlayPrivate() :
    m_altitude( 0.0 ), m_altitudeMode( ClampToGround )
{
    // nothing to do
}

GeoDataGroundOverlay::GeoDataGroundOverlay() : d( new GeoDataGroundOverlayPrivate )
{
    // nothing to do
}

GeoDataGroundOverlay::GeoDataGroundOverlay( const Marble::GeoDataGroundOverlay &other ) :
    GeoDataOverlay( other ), d( new GeoDataGroundOverlayPrivate( *other.d ) )
{
    // nothing to do
}

GeoDataGroundOverlay &GeoDataGroundOverlay::operator=( const GeoDataGroundOverlay &other )
{
    *d = *other.d;
    return *this;
}

GeoDataGroundOverlay::~GeoDataGroundOverlay()
{
    delete d;
}

const char *GeoDataGroundOverlay::nodeType() const
{
    return GeoDataTypes::GeoDataGroundOverlayType;
}

double GeoDataGroundOverlay::altitude() const
{
    return d->m_altitude;
}

void GeoDataGroundOverlay::setAltitude( double altitude )
{
    d->m_altitude = altitude;
}

AltitudeMode GeoDataGroundOverlay::altitudeMode() const
{
    return d->m_altitudeMode;
}

void GeoDataGroundOverlay::setAltitudeMode( const AltitudeMode altitudeMode )
{
    d->m_altitudeMode = altitudeMode;
}

GeoDataLatLonBox &GeoDataGroundOverlay::latLonBox() const
{
    return d->m_latLonBox;
}

void GeoDataGroundOverlay::setLatLonBox( const GeoDataLatLonBox &box )
{
    d->m_latLonBox = box;
}

}
