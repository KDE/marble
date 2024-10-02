// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009-2010 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_SCREENGRAPHICSITEMPRIVATE_H
#define MARBLE_SCREENGRAPHICSITEMPRIVATE_H

#include "MarbleDebug.h"
#include "MarbleGraphicsItem_p.h"

#include "BillboardGraphicsItem.h"
#include "ScreenGraphicsItem.h"
#include "ViewportParams.h"

namespace Marble
{

class ScreenGraphicsItemPrivate : public MarbleGraphicsItemPrivate
{
public:
    ScreenGraphicsItemPrivate(ScreenGraphicsItem *screenGraphicsItem, MarbleGraphicsItem *parent)
        : MarbleGraphicsItemPrivate(screenGraphicsItem, parent)
        , m_position()
        , m_viewportSize()
        , m_floatItemMoving(false)
        , m_flags(ScreenGraphicsItem::GraphicsItemFlags())
    {
    }

    ~ScreenGraphicsItemPrivate() override = default;

    QList<QPointF> positions() const override
    {
        QList<QPointF> list;

        list.append(positivePosition());
        return list;
    }

    QPointF positivePosition() const
    {
        const QSizeF parentSize = m_parent ? m_parent->size() : m_viewportSize;
        if (!parentSize.isValid()) {
            mDebug() << "Invalid parent size";
            return m_position;
        }
        QPointF position;
        qreal x = m_position.x();
        qreal y = m_position.y();

        position.setX((x >= 0) ? x : parentSize.width() + x - m_size.width());
        position.setY((y >= 0) ? y : parentSize.height() + y - m_size.height());

        return position;
    }

    QList<QPointF> absolutePositions() const override
    {
        if (m_parent == nullptr) {
            return positions();
        }

        QList<QPointF> parentPositions;

        if (auto *screenItem = dynamic_cast<ScreenGraphicsItem *>(m_parent)) {
            parentPositions = screenItem->absolutePositions();
        } else if (auto *geoLabelItem = dynamic_cast<BillboardGraphicsItem *>(m_parent)) {
            parentPositions = geoLabelItem->positions();
        }

        QPointF relativePosition = positivePosition();

        QList<QPointF> absolutePositions;
        absolutePositions.reserve(parentPositions.size());
        for (const QPointF &point : std::as_const(parentPositions)) {
            absolutePositions.append(point + relativePosition);
        }

        return absolutePositions;
    }

    void setProjection(const ViewportParams *viewport) override
    {
        // If we have no parent
        if (m_parent == nullptr) {
            // Saving the screen size needed for positions()
            m_viewportSize = viewport->size();
        }
    }

    bool isMovable() const
    {
        return (m_flags & ScreenGraphicsItem::ItemIsMovable) ? true : false;
    }

    QPointF m_position;
    // The size of the parent, or if no parent exists the size of the viewport.
    QSizeF m_viewportSize;

    QPoint m_floatItemMoveStartPos;
    bool m_floatItemMoving;

    ScreenGraphicsItem::GraphicsItemFlags m_flags;
};

}

#endif
