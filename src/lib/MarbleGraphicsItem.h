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

class MarbleGraphicsItem {
 public:
    MarbleGraphicsItem();
    ~MarbleGraphicsItem();
    
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
     * Returns the coordinates of the item in view coordinates
     */
    QPointF pos() const;
    
    /**
     * Set the position of the item
     */
    void setPos( const QPointF& position );
    
    /**
     * Returns the bounding rect of the item in view coordinates
     */
    QRectF boundingRect() const;
    
 protected:
    /**
     * Set the bounding rect of the item
     */
    void setBoundingRect() const; // Should this be protected? I don't think the outer world would want to change the size. All this could be implemented in the child if somebody would like to do this.
     
    bool eventFilter( QObject *object, QEvent *e );
    
 private:
    MarbleGraphicsItemPrivate *d;
};

#endif // MARBLEGRAPHICSITEM_H