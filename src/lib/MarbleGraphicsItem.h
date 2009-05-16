//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLEGRAPHICSITEM_H
#define MARBLEGRAPHICSITEM_H

class QPoint;
class QString;

namespace Marble {
    
class GeoPainter;
class GeoSceneLayer;
class ViewportParams;

class MarbleGraphicsItemPrivate;

class MarbleGraphicsItem {
 public:
    MarbleGraphicsItem();
    virtual ~MarbleGraphicsItem();
    
    /**
     * Paints the item
     */
    virtual bool paint( GeoPainter *painter, ViewportParams *viewport,
                        const QString& renderPos, GeoSceneLayer * layer = 0 ) = 0;
    
    /**
     * Returns true if the Item contains @p point in view coordinates.
     */
    bool contains( const QPoint& point ) const;
    
    /**
     * Returns all coordinates of the item in view coordinates
     */
    QList<QPoint> positions() const;
    
    /**
     * Returns the first coordinate of the item in view coordinates
     */
    QPoint position() const;
    
    /**
     * Returns all bounding rects of the item in view coordinates
     */
    QList<QRect> boundingRects() const;
    
    /**
     * Returns the first bounding rect of the item in view coordinates.
     * Keep in mind boundingRects which will probably be the best choice.
     */
    QRect boundingRect() const;
    
    /**
     * Returns the size of the item
     */
    virtual QSize size() const;
    
 protected:
    /**
     * Set the position of the item
     */
    void setPosition( const QPoint& position );
    
    /**
     * Set the position of the item
     */
    void setPositions( const QList<QPoint>& positions );
    
    /**
     * Set the bounding rect of the item
     */
    void setBoundingRect( const QRect& boundingRect );
    
    /**
     * Set the bounding rect of the item
     */
    void setBoundingRects( const QList<QRect>& boundingRects );
    
    virtual bool eventFilter( QObject *object, QEvent *e );
    
 private:
    MarbleGraphicsItemPrivate *d;
};

} // Namespace Marble

#endif // MARBLEGRAPHICSITEM_H
