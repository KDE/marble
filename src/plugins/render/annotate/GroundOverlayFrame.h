//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Adrian Draghici <draghici.adrian.b@gmail.com>
//

#ifndef MARBLE_GROUNDOVERLAYFRAME_H
#define MARBLE_GROUNDOVERLAYFRAME_H

#include "SceneGraphicsItem.h"
#include "GeoDataGroundOverlay.h"
#include "TextureLayer.h"

namespace Marble
{

class GroundOverlayFrame : public SceneGraphicsItem
{
public:
    GroundOverlayFrame( GeoDataPlacemark *placemark, GeoDataGroundOverlay *overlay, TextureLayer *textureLayer );

    enum MovedRegion {
        NorthWest,
        SouthWest,
        SouthEast,
        NorthEast,
        Polygon
    };

    virtual const char *graphicType() const;

    void update();

private:
    virtual void paint( GeoPainter *painter, const ViewportParams *viewport );
    virtual bool mousePressEvent( QMouseEvent *event );
    virtual bool mouseMoveEvent( QMouseEvent *event );
    virtual bool mouseReleaseEvent( QMouseEvent *event );


    int m_movedPoint;
    GeoDataCoordinates m_movedPointCoordinates;
    GeoDataGroundOverlay *m_overlay;
    TextureLayer *m_textureLayer;
    const ViewportParams *m_viewport;

    static void rotateAroundCenter( qreal lon, qreal lat, qreal &rotatedLon, qreal &rotatedLat, GeoDataLatLonBox &box, bool inverse = false );
};

}

#endif
