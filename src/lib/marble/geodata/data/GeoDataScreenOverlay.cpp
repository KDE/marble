//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "GeoDataScreenOverlay.h"
#include "GeoDataTypes.h"

namespace Marble {

class GeoDataScreenOverlayPrivate
{
public:
    GeoDataScreenOverlayPrivate();

    GeoDataVec2  m_overlayXY;
    GeoDataVec2  m_screenXY;
    GeoDataVec2  m_rotationXY;
    GeoDataVec2  m_size;
    qreal        m_rotation;
};

GeoDataScreenOverlayPrivate::GeoDataScreenOverlayPrivate() :
    m_rotation(0.0)
{
}

GeoDataScreenOverlay::GeoDataScreenOverlay() :
    d( new GeoDataScreenOverlayPrivate )
{
}

GeoDataScreenOverlay::GeoDataScreenOverlay( const Marble::GeoDataScreenOverlay &other ) :
    GeoDataOverlay( other ), d( new GeoDataScreenOverlayPrivate( *other.d ) )
{
}

GeoDataScreenOverlay &GeoDataScreenOverlay::operator=( const GeoDataScreenOverlay &other )
{
    *d = *other.d;
    return *this;
}

GeoDataScreenOverlay::~GeoDataScreenOverlay()
{
    delete d;
}

const char *GeoDataScreenOverlay::nodeType() const
{
    return GeoDataTypes::GeoDataScreenOverlayType;
}

GeoDataVec2 GeoDataScreenOverlay::overlayXY() const
{
    return d->m_overlayXY;
}

void GeoDataScreenOverlay::setOverlayXY(const GeoDataVec2 &vec2)
{
    d->m_overlayXY = vec2;
}

void GeoDataScreenOverlay::setOverlayXY(const qreal &x, const qreal &y, const GeoDataVec2::Unit &xunit,
                                        const GeoDataVec2::Unit &yunit)
{
    d->m_screenXY.setX(x);
    d->m_screenXY.setY(y);
    d->m_screenXY.setXunits(xunit);
    d->m_screenXY.setYunits(yunit);
}

GeoDataVec2 GeoDataScreenOverlay::screenXY() const
{
    return d->m_screenXY;
}

void GeoDataScreenOverlay::setScreenXY(const GeoDataVec2 &vec2)
{
    d->m_screenXY = vec2;
}

void GeoDataScreenOverlay::setScreenXY(const qreal &x, const qreal &y, const GeoDataVec2::Unit &xunit,
                                       const GeoDataVec2::Unit &yunit)
{
    d->m_screenXY.setX(x);
    d->m_screenXY.setY(y);
    d->m_screenXY.setXunits(xunit);
    d->m_screenXY.setYunits(yunit);
}

GeoDataVec2 GeoDataScreenOverlay::rotationXY() const
{
    return d->m_rotationXY;
}

void GeoDataScreenOverlay::setRotationXY(const GeoDataVec2 &vec2)
{
    d->m_rotationXY = vec2;
}

void GeoDataScreenOverlay::setRotationXY(const qreal &x, const qreal &y, const GeoDataVec2::Unit &xunit,
                                         const GeoDataVec2::Unit &yunit)
{
    d->m_rotationXY.setX(x);
    d->m_rotationXY.setY(y);
    d->m_rotationXY.setXunits(xunit);
    d->m_rotationXY.setYunits(yunit);
}

GeoDataVec2 GeoDataScreenOverlay::size() const
{
    return d->m_size;
}

void GeoDataScreenOverlay::setSize(const GeoDataVec2 &vec2)
{
    d->m_size = vec2;
}

void GeoDataScreenOverlay::setSize(const qreal &x, const qreal &y, const GeoDataVec2::Unit &xunit,
                                   const GeoDataVec2::Unit &yunit)
{
    d->m_size.setX(x);
    d->m_size.setY(y);
    d->m_size.setXunits(xunit);
    d->m_size.setYunits(yunit);
}

qreal GeoDataScreenOverlay::rotation() const
{
    return d->m_rotation;
}

void GeoDataScreenOverlay::setRotation(qreal rotation)
{
    d->m_rotation = rotation;
}

}
