//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_BILLBOARDGRAPHICSITEM_H
#define MARBLE_BILLBOARDGRAPHICSITEM_H

#include "MarbleGraphicsItem.h"
#include "marble_export.h"

#include <QtCore/QList>
#include <QtCore/QPointF>

#include "GeoDataCoordinates.h"

namespace Marble
{

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

    /**
     * @brief Returns the absolute screen positions of the item.
     *
     * All elements indicate the position of the topleft corner in screen coordinates and are
     * based on the viewport of the last paintEvent(). Note that the result list may contain
     * an unbound number of elements, including zero, depending on the item's visibility,
     * the projection and zoom level.
     */
    QList<QPointF> positions() const;

    /**
     * @brief Returns the absolute screen bounding boxes of the item.
     */
    QList<QRectF> boundingRects() const;

 private:
    class Private;
    Private *p() const;
};

} // Marble namespace

#endif
