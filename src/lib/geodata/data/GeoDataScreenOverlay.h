//
// This file is part of the Marble Virtual Globe->
//
// This program is free software licensed under the GNU LGPL-> You can
// find a copy of this license in LICENSE->txt in the top directory of
// the source code->
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef GEODATASCREENOVERLAY_H
#define GEODATASCREENOVERLAY_H

#include "GeoDataOverlay.h"
#include "GeoDataVec2.h"
#include "MarbleGlobal.h"

namespace Marble {

class GeoDataScreenOverlayPrivate;

class GeoDataScreenOverlay: public GeoDataOverlay
{
public:
    GeoDataScreenOverlay();

    GeoDataScreenOverlay( const GeoDataScreenOverlay &other );

    GeoDataScreenOverlay& operator=( const GeoDataScreenOverlay &other );

    ~GeoDataScreenOverlay();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    GeoDataVec2 overlayXY() const;
    void setOverlayXY( const GeoDataVec2 &vec2 );
    void setOverlayXY( const qreal &x, const qreal &y, const GeoDataVec2::Unit &xunit, const GeoDataVec2::Unit &yunit );

    GeoDataVec2 screenXY() const;
    void setScreenXY( const GeoDataVec2 &vec2 );
    void setScreenXY( const qreal &x, const qreal &y, const GeoDataVec2::Unit &xunit, const GeoDataVec2::Unit &yunit );

    GeoDataVec2 rotationXY() const;
    void setRotationXY( const GeoDataVec2 &vec2 );
    void setRotationXY( const qreal &x, const qreal &y, const GeoDataVec2::Unit &xunit, const GeoDataVec2::Unit &yunit );

    GeoDataVec2 size() const;
    void setSize( const GeoDataVec2 &vec2 );
    void setSize( const qreal &x, const qreal &y, const GeoDataVec2::Unit &xunit, const GeoDataVec2::Unit &yunit );

    qreal rotation() const;
    void setRotation( qreal rotation );

private:
    GeoDataScreenOverlayPrivate* const d;
};

}

#endif
