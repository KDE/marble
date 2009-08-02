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

#include "marble_export.h"

#include <QtCore/QPointF>
#include <QtCore/QList>
#include <QtCore/QSizeF>

class QBrush;
class QEvent;
class QPainterPath;
class QObject;
class QRectF;
class QString;

namespace Marble
{
    
class GeoPainter;
class GeoSceneLayer;
class ViewportParams;

class MarbleGraphicsItemPrivate;

class MARBLE_EXPORT MarbleGraphicsItem
{
    friend class MarbleGraphicsItemPrivate;

 public:
    enum CacheMode {
        NoCache,
        ItemCoordinateCache,
        DeviceCoordinateCache
    };
     
    explicit MarbleGraphicsItem( MarbleGraphicsItem *parent = 0 );
    
    virtual ~MarbleGraphicsItem();
    
    /**
     * Paints the item on the screen in view coordinates.
     * It is not save to call this function from a thread other than the gui thread.
     */
    bool paintEvent( GeoPainter *painter, ViewportParams *viewport, 
                     const QString& renderPos, GeoSceneLayer *layer = 0 );
                        
    /**
     * Returns true if the Item contains @p point in parent coordinates.
     */
    bool contains( const QPointF& point ) const;

    /**
     * Returns the rect of one represenation of the object that is at the given position.
     */
    QRectF containsRect( const QPointF& point ) const;
    
    /**
     * Returns the size of the item
     */
    QSizeF size() const;
    
    /**
     * Returns the cache mode of the item
     */
    CacheMode cacheMode() const;
    
    /**
     * Set the cache mode of the item
     */
    void setCacheMode( CacheMode mode, const QSize & logicalCacheSize = QSize() );

    /**
     * Schedules an painting update for the Item. As long it is not added to an GraphicsScene
     * (which doesn't exist yet) it will be repainted at the next paint event instead of using
     * the cache.
     */
    void update();

    /**
     * Returns if the item is visible.
     */
    bool visible() const;

    /**
     * Makes the item visible or invisible, depending on @p visible.
     */
    void setVisible( bool visible );

    /**
     * Hides the item. Equivalent to setVisible( false )
     */
    void hide();

    /**
     * Shows the item. Equivalent to setVisible( true )
     */
    void show();
    
    /**
     * Returns the items tool tip or, if no tool tip has been set, an empty string.
     */
    QString toolTip() const;
    
    /**
     * Set the tool tip for this GraphicItem.
     */
    void setToolTip( const QString& toolTip );
    
 protected:
    explicit MarbleGraphicsItem( MarbleGraphicsItemPrivate *d_ptr );

    /**
     * Paints the item in item coordinates. This has to be reimplemented by the subclass
     * This function will be called by paintEvent().
     */
    virtual void paint( GeoPainter *painter, ViewportParams *viewport,
                        const QString& renderPos, GeoSceneLayer * layer = 0 ) = 0;
     
    virtual bool eventFilter( QObject *object, QEvent *e );
    
    /**
     * Set the size of the item
     */
    void setSize( const QSizeF& size );
    
    MarbleGraphicsItemPrivate * const d;
    
 private:
    MarbleGraphicsItemPrivate* p() const;
};

} // Namespace Marble

#endif // MARBLEGRAPHICSITEM_H
