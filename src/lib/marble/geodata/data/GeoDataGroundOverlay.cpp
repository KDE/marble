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

    GeoDataLatLonQuad m_latLonQuad;

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

bool GeoDataGroundOverlay::operator==(const GeoDataGroundOverlay& other) const
{
    return equals(other) &&
           d->m_altitude == other.d->m_altitude &&
           d->m_altitudeMode == other.d->m_altitudeMode &&
           d->m_latLonBox == other.d->m_latLonBox &&
           d->m_latLonQuad == other.d->m_latLonQuad;
}

bool GeoDataGroundOverlay::operator!=(const GeoDataGroundOverlay& other) const
{
    return !this->operator==(other);
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

GeoDataLatLonQuad &GeoDataGroundOverlay::latLonQuad()
{
    return d->m_latLonQuad;
}

const GeoDataLatLonQuad &GeoDataGroundOverlay::latLonQuad() const
{
    return d->m_latLonQuad;
}

void GeoDataGroundOverlay::setLatLonQuad(const GeoDataLatLonQuad& quad)
{
    d->m_latLonQuad = quad;
}

}
