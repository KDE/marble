// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_SCREENGRAPHICSITEM_H
#define MARBLE_SCREENGRAPHICSITEM_H

// Marble
#include "MarbleGraphicsItem.h"
#include "marble_export.h"

// Qt
#include <QVector>

namespace Marble
{

class ScreenGraphicsItemPrivate;

class MARBLE_EXPORT ScreenGraphicsItem : public MarbleGraphicsItem
{
 public:
    enum GraphicsItemFlag {
        ItemIsMovable = 0x1,
        ItemIsHideable = 0x2
    };
    Q_DECLARE_FLAGS(GraphicsItemFlags, GraphicsItemFlag)

    explicit ScreenGraphicsItem( MarbleGraphicsItem *parent = nullptr );

    ~ScreenGraphicsItem() override;

    /**
     * @brief Set the position of the ScreenGraphicsItem
     * @param position Position
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
     * @since 0.26.0
     */
    QVector<QPointF> absolutePositions() const;

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
    explicit ScreenGraphicsItem(ScreenGraphicsItemPrivate *dd);

    bool eventFilter( QObject *, QEvent * ) override;

 private:
    Q_DECLARE_PRIVATE(ScreenGraphicsItem)
};

} // Namespace Marble

#endif
