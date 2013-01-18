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


#ifndef MARBLE_GEODATAPHOTOOVERLAY_H
#define MARBLE_GEODATAPHOTOOVERLAY_H

#include "GeoDataOverlay.h"
#include "MarbleGlobal.h"
#include "GeoDataPoint.h"
#include "GeoDataImagePyramid.h"
#include "GeoDataViewVolume.h"
#include "marble_export.h"

namespace Marble {

class GeoDataPhotoOverlayPrivate;

class MARBLE_EXPORT GeoDataPhotoOverlay: public GeoDataOverlay
{
public:
    GeoDataPhotoOverlay();

    GeoDataPhotoOverlay( const GeoDataPhotoOverlay &other );

    GeoDataPhotoOverlay& operator=( const GeoDataPhotoOverlay &other );

    ~GeoDataPhotoOverlay();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    enum Shape {
        Rectangle,
        Cylinder,
        Sphere
    };

    qreal rotation() const;
    void setRotation( const qreal &rotation );

    GeoDataViewVolume &viewVolume();
    const GeoDataViewVolume& viewVolume() const;
    void setViewVolume( const GeoDataViewVolume &viewVolume );

    GeoDataImagePyramid& imagePyramid();
    const GeoDataImagePyramid& imagePyramid() const;
    void setImagePyramid( const GeoDataImagePyramid &imagePyramid );

    GeoDataPoint& point() const;
    void setPoint( const GeoDataPoint &point );

    Shape& shape();
    const Shape& shape() const;
    void setShape( const Shape &shape );

private:
    GeoDataPhotoOverlayPrivate* const d;
};

}

#endif
