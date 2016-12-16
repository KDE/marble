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
#include "GeoDataCoordinates.h"

namespace Marble
{
class TextureLayer;
class GeoDataGroundOverlay;

class GroundOverlayFrame : public SceneGraphicsItem
{
public:
    GroundOverlayFrame( GeoDataPlacemark *placemark, GeoDataGroundOverlay *overlay, TextureLayer *textureLayer );

    enum MovedRegion {
        NoRegion = -1,
        NorthWest = 0,
        SouthWest,
        SouthEast,
        NorthEast,
        North,
        South,
        East,
        West,
        Polygon
    };

    enum EditStatus {
        Resize,
        Rotate
    };

    void update();

    bool containsPoint( const QPoint &eventPos ) const override;

    void dealWithItemChange( const SceneGraphicsItem *other ) override;

    void move( const GeoDataCoordinates &source, const GeoDataCoordinates &destination ) override;

    /**
     * @brief Provides information for downcasting a SceneGraphicsItem.
     */
    const char *graphicType() const override;

protected:
    void paint( GeoPainter *painter, const ViewportParams *viewport, const QString &layer, int tileZoomLevel ) override;
    bool mousePressEvent( QMouseEvent *event ) override;
    bool mouseMoveEvent( QMouseEvent *event ) override;
    bool mouseReleaseEvent( QMouseEvent *event ) override;

    void dealWithStateChange( SceneGraphicsItem::ActionState previousState ) override;
private:
    GeoDataGroundOverlay *m_overlay;
    TextureLayer         *m_textureLayer;

    QVector<QRegion>   m_regionList;
    GeoDataCoordinates m_movedHandleGeoCoordinates;
    QPoint             m_movedHandleScreenCoordinates;
    int                m_movedHandle;
    int                m_hoveredHandle;
    int                m_editStatus;
    bool               m_editStatusChangeNeeded;
    qreal              m_previousRotation;

    QVector<QImage>    m_resizeIcons;
    QVector<QImage>    m_rotateIcons;

    const ViewportParams *m_viewport;
};

}

#endif
