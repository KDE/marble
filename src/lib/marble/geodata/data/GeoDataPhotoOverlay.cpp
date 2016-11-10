//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2013      Mohammed Nafees  <nafees.technocool@gmail.com>
//

#include "GeoDataPhotoOverlay.h"
#include "GeoDataPhotoOverlay_p.h"

namespace Marble {

GeoDataPhotoOverlay::GeoDataPhotoOverlay()
  : GeoDataOverlay(new GeoDataPhotoOverlayPrivate)
{
    // nothing to do
}

GeoDataPhotoOverlay::GeoDataPhotoOverlay(const GeoDataPhotoOverlay &other)
  : GeoDataOverlay(other, new GeoDataPhotoOverlayPrivate(*other.d_func()))
{
    // nothing to do
}

GeoDataPhotoOverlay::~GeoDataPhotoOverlay()
{
}

GeoDataPhotoOverlay &GeoDataPhotoOverlay::operator=( const GeoDataPhotoOverlay &other )
{
    if (this != &other) {
        Q_D(GeoDataPhotoOverlay);
        *d = *other.d_func();
    }

    return *this;
}

bool GeoDataPhotoOverlay::operator==(const GeoDataPhotoOverlay& other) const
{
    Q_D(const GeoDataPhotoOverlay);
    const GeoDataPhotoOverlayPrivate* const other_d = other.d_func();

    return equals(other) &&
           d->m_rotation == other_d->m_rotation &&
           d->m_shape == other_d->m_shape &&
           d->m_imagePyramid == other_d->m_imagePyramid &&
           d->m_point == other_d->m_point &&
           d->m_viewVolume == other_d->m_viewVolume;
}

bool GeoDataPhotoOverlay::operator!=(const GeoDataPhotoOverlay& other) const
{
    return !this->operator==(other);
}

GeoDataFeature * GeoDataPhotoOverlay::clone() const
{
    return new GeoDataPhotoOverlay(*this);
}

const char *GeoDataPhotoOverlay::nodeType() const
{
    Q_D(const GeoDataPhotoOverlay);
    return GeoDataTypes::GeoDataPhotoOverlayType;
}

qreal GeoDataPhotoOverlay::rotation() const
{
    Q_D(const GeoDataPhotoOverlay);
    return d->m_rotation;
}

void GeoDataPhotoOverlay::setRotation( const qreal rotation )
{
    Q_D(GeoDataPhotoOverlay);
    d->m_rotation = rotation;
}

GeoDataViewVolume& GeoDataPhotoOverlay::viewVolume()
{
    Q_D(GeoDataPhotoOverlay);
    return d->m_viewVolume;
}

const GeoDataViewVolume& GeoDataPhotoOverlay::viewVolume() const
{
    Q_D(const GeoDataPhotoOverlay);
    return d->m_viewVolume;
}

void GeoDataPhotoOverlay::setViewVolume( const GeoDataViewVolume &viewVolume )
{
    Q_D(GeoDataPhotoOverlay);
    d->m_viewVolume = viewVolume;
}

GeoDataImagePyramid& GeoDataPhotoOverlay::imagePyramid()
{
    Q_D(GeoDataPhotoOverlay);
    return d->m_imagePyramid;
}

const GeoDataImagePyramid& GeoDataPhotoOverlay::imagePyramid() const
{
    Q_D(const GeoDataPhotoOverlay);
    return d->m_imagePyramid;
}

void GeoDataPhotoOverlay::setImagePyramid( const GeoDataImagePyramid &imagePyramid )
{
    Q_D(GeoDataPhotoOverlay);
    d->m_imagePyramid = imagePyramid;
}

GeoDataPoint& GeoDataPhotoOverlay::point()
{
    Q_D(GeoDataPhotoOverlay);
    return d->m_point;
}

const GeoDataPoint& GeoDataPhotoOverlay::point() const
{
    Q_D(const GeoDataPhotoOverlay);
    return d->m_point;
}

void GeoDataPhotoOverlay::setPoint( const GeoDataPoint &point )
{
    Q_D(GeoDataPhotoOverlay);
    d->m_point = point;
}

GeoDataPhotoOverlay::Shape GeoDataPhotoOverlay::shape() const
{
    Q_D(const GeoDataPhotoOverlay);
    return d->m_shape;
}

void GeoDataPhotoOverlay::setShape( Shape shape )
{
    Q_D(GeoDataPhotoOverlay);
    d->m_shape = shape;
}

}
