//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_SCREENGRAPHICSITEM_H
#define MARBLE_SCREENGRAPHICSITEM_H

// Marble
#include "MarbleGraphicsItem.h"
#include "marble_export.h"

// Qt
#include <QtCore/QList>

class QPoint;

namespace Marble
{

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

    virtual ~ScreenGraphicsItem();

    /**
     * @brief Set the position of the ScreenGraphicsItem
     * @param pos Position
     *
     * Positive x-coordinates are counted left-aligned from the left border of the parent item.
     * Negative x-coordinates are counted right-aligned from the right border of the parent item.
     *
     * Positive y-coordinates are counted top-aligned from the top border of the parent item.
     * Negative y-coordinates are counted right-aligned from the bottom border of the parent item.
     */
    void    setPosition( const QPointF& position );

    /**
     * @brief Return the position of the ScreenGraphicsItem
     *
     * Positive x-coordinates are counted left-aligned from the left border of the parent item.
     * Negative x-coordinates are counted right-aligned from the right border of the parent item.
     *
     * Positive y-coordinates are counted top-aligned from the top border of the parent item.
     * Negative y-coordinates are counted right-aligned from the bottom border of the parent item.
     */
    QPointF position() const;

    /**
     * @brief Return the positive position of the ScreenGraphicsItem
     *
     * All coordinates are counted positive and depend on the current viewport.
     *
     * Positive x-coordinates are counted left-aligned from the left border of the parent item.
     * Positive y-coordinates are counted top-aligned from the top border of the parent item.
     */
    QPointF positivePosition() const;

    /**
     * @brief Return the absolute position of the ScreenGraphicsItem
     *
     * All coordinates are counted positive and depend on the current viewport.
     *
     * Positive x-coordinates are counted left-aligned from the left border of the map.
     * Positive y-coordinates are counted top-aligned from the top border of the map.
     */
    QList<QPointF> absolutePositions() const;

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

#endif
