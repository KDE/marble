//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2013      Mohammed Nafees  <nafees.technocool@gmail.com>
//

#include "GeoDataPhotoOverlay.h"
#include "GeoDataTypes.h"

namespace Marble {

class GeoDataPhotoOverlayPrivate
{
public:
    GeoDataPhotoOverlayPrivate();

    qreal m_rotation;
    GeoDataViewVolume m_viewVolume;
    GeoDataImagePyramid m_imagePyramid;
    GeoDataPoint m_point;
    GeoDataPhotoOverlay::Shape m_shape;
};

GeoDataPhotoOverlayPrivate::GeoDataPhotoOverlayPrivate() :
    m_rotation( 0.0 ),
    m_viewVolume(),
    m_imagePyramid(),
    m_point(),
    m_shape( GeoDataPhotoOverlay::Rectangle )
{
    // nothing to do
}

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

qreal GeoDataPhotoOverlay::rotation() const
{
    return d->m_rotation;
}

void GeoDataPhotoOverlay::setRotation( const qreal &rotation )
{
    d->m_rotation = rotation;
}

GeoDataViewVolume& GeoDataPhotoOverlay::viewVolume()
{
    return d->m_viewVolume;
}

const GeoDataViewVolume& GeoDataPhotoOverlay::viewVolume() const
{
    return d->m_viewVolume;
}

void GeoDataPhotoOverlay::setViewVolume( const GeoDataViewVolume &viewVolume )
{
    d->m_viewVolume = viewVolume;
}

GeoDataImagePyramid& GeoDataPhotoOverlay::imagePyramid()
{
    return d->m_imagePyramid;
}

const GeoDataImagePyramid& GeoDataPhotoOverlay::imagePyramid() const
{
    return d->m_imagePyramid;
}

void GeoDataPhotoOverlay::setImagePyramid( const GeoDataImagePyramid &imagePyramid )
{
    d->m_imagePyramid = imagePyramid;
}

GeoDataPoint& GeoDataPhotoOverlay::point() const
{
    return d->m_point;
}

void GeoDataPhotoOverlay::setPoint( const GeoDataPoint &point )
{
    d->m_point = point;
}

GeoDataPhotoOverlay::Shape& GeoDataPhotoOverlay::shape()
{
    return d->m_shape;
}

const GeoDataPhotoOverlay::Shape& GeoDataPhotoOverlay::shape() const
{
    return d->m_shape;
}

void GeoDataPhotoOverlay::setShape( const Shape &shape )
{
    d->m_shape = shape;
}

}
