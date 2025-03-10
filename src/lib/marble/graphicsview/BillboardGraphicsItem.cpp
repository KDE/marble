// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

// self
#include "BillboardGraphicsItem.h"
#include "MarbleGraphicsItem_p.h"

// Marble
#include "ViewportParams.h"

namespace Marble
{

class BillboardGraphicsItemPrivate : public MarbleGraphicsItemPrivate
{
public:
    BillboardGraphicsItemPrivate(BillboardGraphicsItem *parent)
        : MarbleGraphicsItemPrivate(parent)
        , m_alignment(Qt::AlignHCenter | Qt::AlignVCenter)
    {
    }

    QList<QPointF> positions() const override
    {
        return m_positions;
    }

    QList<QPointF> absolutePositions() const override
    {
        return m_positions;
    }

    Qt::Alignment m_alignment;

    void setProjection(const ViewportParams *viewport) override
    {
        m_positions.clear();

        qreal x[100], y;
        int pointRepeatNumber = 0;
        bool globeHidesPoint;

        viewport->screenCoordinates(m_coordinates, x, y, pointRepeatNumber, m_size, globeHidesPoint);

        // Don't display items if they are on the far side of the globe.
        if (globeHidesPoint)
            return;

        m_positions.reserve(pointRepeatNumber);
        // handle vertical alignment
        qint32 topY = (viewport->height() - m_size.height()) / 2;
        if (m_alignment & Qt::AlignTop) {
            topY = y - m_size.height();
        } else if (m_alignment & Qt::AlignVCenter) {
            topY = y - (m_size.height() / 2);
        } else if (m_alignment & Qt::AlignBottom) {
            topY = y;
        }

        for (int i = 0; i < pointRepeatNumber; ++i) {
            // handle horizontal alignment
            qint32 leftX = (viewport->width() - m_size.width()) / 2;
            if (m_alignment & Qt::AlignLeft) {
                leftX = x[i] - m_size.width();
            } else if (m_alignment & Qt::AlignHCenter) {
                leftX = x[i] - (m_size.width() / 2);
            } else if (m_alignment & Qt::AlignRight) {
                leftX = x[i];
            }

            m_positions.append(QPoint(leftX, topY));
        }
    }

    GeoDataCoordinates m_coordinates;
    QList<QPointF> m_positions;
};

BillboardGraphicsItem::BillboardGraphicsItem()
    : MarbleGraphicsItem(new BillboardGraphicsItemPrivate(this))
{
}

GeoDataCoordinates BillboardGraphicsItem::coordinate() const
{
    Q_D(const BillboardGraphicsItem);
    return d->m_coordinates;
}

void BillboardGraphicsItem::setCoordinate(const GeoDataCoordinates &coordinates)
{
    Q_D(BillboardGraphicsItem);
    d->m_coordinates = coordinates;
}

QList<QPointF> BillboardGraphicsItem::positions() const
{
    Q_D(const BillboardGraphicsItem);
    return d->positions();
}

QList<QRectF> BillboardGraphicsItem::boundingRects() const
{
    Q_D(const BillboardGraphicsItem);

    QList<QRectF> rects;
    rects.reserve(d->m_positions.size());

    QSizeF const size = d->m_size;
    for (const QPointF &point : d->m_positions) {
        rects << QRectF(point, size);
    }
    return rects;
}

QRectF BillboardGraphicsItem::containsRect(const QPointF &point) const
{
    for (const QRectF &rect : boundingRects()) {
        if (rect.contains(point))
            return rect;
    }

    return {};
}

Qt::Alignment BillboardGraphicsItem::alignment() const
{
    Q_D(const BillboardGraphicsItem);
    return d->m_alignment;
}

void BillboardGraphicsItem::setAlignment(Qt::Alignment alignment)
{
    Q_D(BillboardGraphicsItem);
    d->m_alignment = alignment;
}

} // Marble namespace
