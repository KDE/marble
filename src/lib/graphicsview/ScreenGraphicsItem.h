//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef SCREENGRAPHICSITEM_H
#define SCREENGRAPHICSITEM_H

// Marble
#include "MarbleGraphicsItem.h"
#include "marble_export.h"

// Qt
#include <QtCore/QList>

class QPoint;
class QRect;
class QString;

namespace Marble
{

class GeoPainter;
class ViewportParams;

class ScreenGraphicsItemPrivate;

class MARBLE_EXPORT ScreenGraphicsItem : public MarbleGraphicsItem
{
 public:
    enum GraphicsItemFlag {
        ItemIsMovable = 0x1
    };
    Q_DECLARE_FLAGS(GraphicsItemFlags, GraphicsItemFlag)

    explicit ScreenGraphicsItem( MarbleGraphicsItem *parent = 0 );
    ScreenGraphicsItem( const QPointF& position, const QSizeF& size,
                        MarbleGraphicsItem *parent = 0 );
    
    virtual ~ScreenGraphicsItem();

    /**
     * @brief Set the position of the float item
     * @param pos Position
     *
     * Positive x-coordinates are counted left-aligned from the left map border.
     * Negative x-coordinates are counted right-aligned from the right map border.
     *
     * Positive y-coordinates are counted top-aligned from the top map border.
     * Negative y-coordinates are counted right-aligned from the bottom map border.
     */
    void    setPosition( const QPointF& position );

    /**
     * @brief Return the position of the float item
     *
     * Positive x-coordinates are counted left-aligned from the left map border.
     * Negative x-coordinates are counted right-aligned from the right map border.
     *
     * Positive y-coordinates are counted top-aligned from the top map border.
     * Negative y-coordinates are counted right-aligned from the bottom map border.
     */
    QPointF position() const;

    /**
     * @brief Return the positive position of the float item
     *
     * All coordinates are counted positive and depend on the current viewport.
     *
     * Positive x-coordinates are counted left-aligned from the left map border.
     * Positive y-coordinates are counted top-aligned from the top map border.
     */
    QPointF positivePosition() const;

    virtual void changeViewport( ViewportParams *viewport );

    /**
     * Returns the flags of the item.
     */
    GraphicsItemFlags flags() const;

    /**
     * Sets the flags to flags. All flags in flags will be enabled and all other flags will
     * be disabled. By default all flags are disabled.
     */
    void setFlags( GraphicsItemFlags flags );
    
 protected:
    explicit ScreenGraphicsItem( ScreenGraphicsItemPrivate *d_ptr );
    ScreenGraphicsItem( const QPointF& position, const QSizeF& size,
                        ScreenGraphicsItemPrivate *d_ptr );

    virtual bool eventFilter( QObject *, QEvent * );
     
 private:
    ScreenGraphicsItemPrivate *p() const;
};

} // Namespace Marble

#endif // GEOGRAPHICSITEM_H
