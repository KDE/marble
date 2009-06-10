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

#include <QtCore/QList>
#include <QtCore/QSize>

class QEvent;
class QPoint;
class QObject;
class QString;

namespace Marble {
    
class GeoPainter;
class GeoSceneLayer;
class ViewportParams;

class MarbleGraphicsItemPrivate;

class MARBLE_EXPORT MarbleGraphicsItem {
 public:
    enum CacheMode {
        NoCache,
        ItemCoordinateCache,
        DeviceCoordinateCache
    };
     
    MarbleGraphicsItem();
    MarbleGraphicsItem( MarbleGraphicsItemPrivate *d_ptr );
    
    virtual ~MarbleGraphicsItem();
    
    /**
     * Paints the item on the screen in view coordinates.
     */
    bool paintEvent( GeoPainter *painter, ViewportParams *viewport, 
                     const QString& renderPos, GeoSceneLayer *layer = 0 );
                        
    /**
     * Returns true if the Item contains @p point in view coordinates.
     */
    bool contains( const QPoint& point ) const;
    
    /**
     * Returns the size of the item
     */
    QSize size() const;
    
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
    
 protected:
    /**
     * Paints the item in item coordinates. This has to be reimplemented by the subclass
     * This function will be called by paintEvent().
     */
    virtual void paint( GeoPainter *painter, ViewportParams *viewport,
                        const QString& renderPos, GeoSceneLayer * layer = 0 ) = 0;
     
    virtual bool eventFilter( QObject *object, QEvent *e );
    
    /**
     * Test if an update of the cache is needed.
     * @return true if the item need an update, false otherwise.
     */
    bool needsUpdate();
    
    /**
     * Set the size of the item
     */
    void setSize( const QSize& size );
    
    MarbleGraphicsItemPrivate * const d;
    
 private:
    MarbleGraphicsItemPrivate* p() const;
};

} // Namespace Marble

#endif // MARBLEGRAPHICSITEM_H
