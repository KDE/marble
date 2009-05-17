//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef GEOGRAPHICSITEM_H
#define GEOGRAPHICSITEM_H

namespace Marble {

class GeoGraphicsItem : public MarbleGraphicsItem {
 public:
    GeoGraphicsItem();
    virtual ~GeoGraphicsItem();
    
    /**
     * Paints all instances of the item.
     * Remember: One position can be shown multiple times on a flat map
     */
    bool paint( GeoPainter *painter, ViewportParams *viewport,
                const QString& renderPos, GeoSceneLayer * layer = 0 );
    
    /**
     * Return the coordinate of the item as a GeoDataCoordinates
     */
    GeoDataCoordinates coordinate() const;
                        
    /**
     * Return the coordinate of the item as @p longitude
     * and @p latitude.
     */
    void coordinate( qreal &longitude, qreal &latitude, qreal &altitude ) const;

    /**
     * Set the coordinate of the item in @p longitude and
     * @p latitude.
     */
    void setCoordinate( qreal longitude, qreal latitude, qreal altitude = 0 );
    
    /**
     * Set the coordinate of the item with an @p GeoDataPoint.
     */
    void setCoordinate( const GeoDataPoint &point );
    
    /**
     * Get the target of the item. The target is the current planet string.s
     */
    QString target();
    
    /**
     * Set the target of the item with @p target.
     */
    void setTarget( const QString& target );
    
    /**
     * Returns all coordinates of the item in view coordinates according to the given projection.
     */
    QList<QPoint> positions( AbstractProjection* projection );

 protected:
    /**
     * Paints one instance of the item to the position @p paintRect.
     */
    virtual bool paint( GeoPainter *painter, const QRect& paintRect ) = 0;
    
 private:
    void setBoundingRects( const QList<QRectF>& boundingRect );
    void setPositions( const QList<QPointF>& position );
    
    void setBoundingRect( const QRectF& boundingRect );
    void setPosition( const QPointF& position );
     
    GeoGraphicsItemPrivate *d;
};

} // Namespace Marble

#endif // GEOGRAPHICSITEM_H

