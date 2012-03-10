//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_GEODATAPHOTOOVERLAY_H
#define MARBLE_GEODATAPHOTOOVERLAY_H

#include "GeoDataOverlay.h"
#include "global.h"
#include "GeoDataPoint.h"

namespace Marble {

class GeoDataPhotoOverlayPrivate;

class GeoDataPhotoOverlay: public GeoDataOverlay
{
public:
    GeoDataPhotoOverlay();

    GeoDataPhotoOverlay( const GeoDataPhotoOverlay &other );

    GeoDataPhotoOverlay& operator=( const GeoDataPhotoOverlay &other );

    ~GeoDataPhotoOverlay();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    GeoDataPoint& point() const;

    void setPoint( const GeoDataPoint &point );

private:
    GeoDataPhotoOverlayPrivate* const d;
};

}

#endif
