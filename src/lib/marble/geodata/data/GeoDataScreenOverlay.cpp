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
#include "GeoDataScreenOverlay_p.h"
#include "GeoDataTypes.h"

namespace Marble {

GeoDataScreenOverlay::GeoDataScreenOverlay()
  : GeoDataOverlay(new GeoDataScreenOverlayPrivate)
{
}

GeoDataScreenOverlay::GeoDataScreenOverlay( const Marble::GeoDataScreenOverlay &other )
  : GeoDataOverlay(other, new GeoDataScreenOverlayPrivate(*other.d_func()))
{
}

GeoDataScreenOverlay::~GeoDataScreenOverlay()
{
}

GeoDataScreenOverlay &GeoDataScreenOverlay::operator=( const GeoDataScreenOverlay &other )
{
    if (this != &other) {
        Q_D(GeoDataScreenOverlay);
        *d = *other.d_func();
    }

    return *this;
}

bool GeoDataScreenOverlay::operator==(const GeoDataScreenOverlay& other) const
{
    Q_D(const GeoDataScreenOverlay);
    const GeoDataScreenOverlayPrivate* const other_d = other.d_func();

    return equals(other) &&
           d->m_overlayXY == other_d->m_overlayXY &&
           d->m_screenXY == other_d->m_screenXY &&
           d->m_rotationXY == other_d->m_rotationXY &&
           d->m_size == other_d->m_size &&
           d->m_rotation == other_d->m_rotation;
}

bool GeoDataScreenOverlay::operator!=(const GeoDataScreenOverlay& other) const
{
    return !this->operator==(other);
}

GeoDataFeature * GeoDataScreenOverlay::clone() const
{
    return new GeoDataScreenOverlay(*this);
}

const char *GeoDataScreenOverlay::nodeType() const
{
    return GeoDataTypes::GeoDataScreenOverlayType;
}

GeoDataVec2 GeoDataScreenOverlay::overlayXY() const
{
    Q_D(const GeoDataScreenOverlay);
    return d->m_overlayXY;
}

void GeoDataScreenOverlay::setOverlayXY(const GeoDataVec2 &vec2)
{
    Q_D(GeoDataScreenOverlay);
    d->m_overlayXY = vec2;
}

void GeoDataScreenOverlay::setOverlayXY(qreal x, qreal y, GeoDataVec2::Unit xunit, GeoDataVec2::Unit yunit)
{
    Q_D(GeoDataScreenOverlay);
    d->m_screenXY.setX(x);
    d->m_screenXY.setY(y);
    d->m_screenXY.setXunits(xunit);
    d->m_screenXY.setYunits(yunit);
}

GeoDataVec2 GeoDataScreenOverlay::screenXY() const
{
    Q_D(const GeoDataScreenOverlay);
    return d->m_screenXY;
}

void GeoDataScreenOverlay::setScreenXY(const GeoDataVec2 &vec2)
{
    Q_D(GeoDataScreenOverlay);
    d->m_screenXY = vec2;
}

void GeoDataScreenOverlay::setScreenXY(qreal x, qreal y, GeoDataVec2::Unit xunit, GeoDataVec2::Unit yunit)
{
    Q_D(GeoDataScreenOverlay);
    d->m_screenXY.setX(x);
    d->m_screenXY.setY(y);
    d->m_screenXY.setXunits(xunit);
    d->m_screenXY.setYunits(yunit);
}

GeoDataVec2 GeoDataScreenOverlay::rotationXY() const
{
    Q_D(const GeoDataScreenOverlay);
    return d->m_rotationXY;
}

void GeoDataScreenOverlay::setRotationXY(const GeoDataVec2 &vec2)
{
    Q_D(GeoDataScreenOverlay);
    d->m_rotationXY = vec2;
}

void GeoDataScreenOverlay::setRotationXY(qreal x, qreal y, GeoDataVec2::Unit xunit, GeoDataVec2::Unit yunit)
{
    Q_D(GeoDataScreenOverlay);
    d->m_rotationXY.setX(x);
    d->m_rotationXY.setY(y);
    d->m_rotationXY.setXunits(xunit);
    d->m_rotationXY.setYunits(yunit);
}

GeoDataVec2 GeoDataScreenOverlay::size() const
{
    Q_D(const GeoDataScreenOverlay);
    return d->m_size;
}

void GeoDataScreenOverlay::setSize(const GeoDataVec2 &vec2)
{
    Q_D(GeoDataScreenOverlay);
    d->m_size = vec2;
}

void GeoDataScreenOverlay::setSize(qreal x, qreal y, GeoDataVec2::Unit xunit, GeoDataVec2::Unit yunit)
{
    Q_D(GeoDataScreenOverlay);
    d->m_size.setX(x);
    d->m_size.setY(y);
    d->m_size.setXunits(xunit);
    d->m_size.setYunits(yunit);
}

qreal GeoDataScreenOverlay::rotation() const
{
    Q_D(const GeoDataScreenOverlay);
    return d->m_rotation;
}

void GeoDataScreenOverlay::setRotation(qreal rotation)
{
    Q_D(GeoDataScreenOverlay);
    d->m_rotation = rotation;
}

}
