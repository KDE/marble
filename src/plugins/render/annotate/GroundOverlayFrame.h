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

    void update();

    virtual bool containsPoint( const QPoint &eventPos ) const;

    virtual void dealWithItemChange( const SceneGraphicsItem *other );

    /**
     * @brief Provides information for downcasting a SceneGraphicsItem.
     */
    virtual const char *graphicType() const;

protected:
    virtual void paint( GeoPainter *painter, const ViewportParams *viewport );
    virtual bool mousePressEvent( QMouseEvent *event );
    virtual bool mouseMoveEvent( QMouseEvent *event );
    virtual bool mouseReleaseEvent( QMouseEvent *event );

    virtual void dealWithStateChange( SceneGraphicsItem::ActionState previousState );
private:
    GeoDataGroundOverlay *m_overlay;
    TextureLayer         *m_textureLayer;

    QList<QRegion>     m_regionList;
    GeoDataCoordinates m_movedPointCoordinates;
    int                m_movedPoint;

    const ViewportParams *m_viewport;

    static void rotateAroundCenter( qreal lon, qreal lat, qreal &rotatedLon, qreal &rotatedLat,
                                    GeoDataLatLonBox &box, bool inverse = false );
};

}

#endif
