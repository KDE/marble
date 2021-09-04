// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_BILLBOARDGRAPHICSITEM_H
#define MARBLE_BILLBOARDGRAPHICSITEM_H

#include "MarbleGraphicsItem.h"
#include "marble_export.h"

#include <Qt>

class QPointF;

namespace Marble
{
class GeoDataCoordinates;
class BillboardGraphicsItemPrivate;

/**
 * @brief Base class for all 2D labels (a.k.a. billboards) in 3D space.
 *
 * A BillboardGraphicsItem represents a point of interest on a planet with a geo location.
 * As a result, it may appear more than once on the map, depending on the projection and
 * the zoom level.
 */
class MARBLE_EXPORT BillboardGraphicsItem : public MarbleGraphicsItem
{
 public:
    BillboardGraphicsItem();

    GeoDataCoordinates coordinate() const;
    void setCoordinate( const GeoDataCoordinates &coordinates );

    Qt::Alignment alignment() const;
    void setAlignment( Qt::Alignment alignment );

    /**
     * @brief Returns the absolute screen positions of the item.
     *
     * All elements indicate the position of the topleft corner in screen coordinates and are
     * based on the viewport of the last paintEvent(). Note that the result list may contain
     * an unbound number of elements, including zero, depending on the item's visibility,
     * the projection and zoom level.
     * @since 0.26.0
     */
    QVector<QPointF> positions() const;

    /**
     * @brief Returns the absolute screen bounding boxes of the item.
     * @since 0.26.0
     */
    QVector<QRectF> boundingRects() const;

    /**
     * Returns the rect of one representation of the object that is at the given position.
     */
    QRectF containsRect( const QPointF &point ) const;

 private:
    Q_DECLARE_PRIVATE(BillboardGraphicsItem)
};

} // Marble namespace

#endif
