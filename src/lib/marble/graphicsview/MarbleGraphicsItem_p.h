// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009-2010 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_MARBLEGRAPHICSITEMPRIVATE_H
#define MARBLE_MARBLEGRAPHICSITEMPRIVATE_H

// Marble
#include "AbstractMarbleGraphicsLayout.h"
#include "MarbleGraphicsItem.h"

// Qt
#include <QDebug>
#include <QList>
#include <QPixmap>
#include <QRect>
#include <QSet>
#include <QSize>
#include <QSizeF>

namespace Marble
{

class MarbleGraphicsItemPrivate
{
public:
    explicit MarbleGraphicsItemPrivate(MarbleGraphicsItem *marbleGraphicsItem, MarbleGraphicsItem *parent = nullptr)
        : m_repaintNeeded(true)
        , m_cacheMode(MarbleGraphicsItem::NoCache)
        , m_visibility(true)
        , m_parent(parent)
        , m_children()
        , m_layout(nullptr)
        , m_marbleGraphicsItem(marbleGraphicsItem)
    {
        if (m_parent) {
            m_parent->d_func()->addChild(m_marbleGraphicsItem);
        }
    }

    virtual ~MarbleGraphicsItemPrivate()
    {
        // Remove from parent
        if (m_parent) {
            m_parent->d_func()->removeChild(m_marbleGraphicsItem);
        }

        // Delete all children
        qDeleteAll(m_children.values()); // delete using a copy, since children may invalidate m_children's iterator

        // Delete Layout
        delete m_layout;
    }

    void addChild(MarbleGraphicsItem *child)
    {
        m_children.insert(child);
    }

    void removeChild(MarbleGraphicsItem *child)
    {
        m_children.remove(child);
    }

    virtual QList<QPointF> positions() const = 0;

    virtual QList<QPointF> absolutePositions() const = 0;

    /**
     * @brief Used to get the set of screen bounding rects
     */
    QList<QRectF> boundingRects() const;

    virtual void setProjection(const ViewportParams *viewport) = 0;

    void updateChildPositions()
    {
        // This has to be done recursively because we need a correct size from all children.
        for (MarbleGraphicsItem *item : std::as_const(m_children)) {
            item->d_func()->updateChildPositions();
        }

        // Adjust positions
        if (m_layout) {
            m_layout->updatePositions(m_marbleGraphicsItem);
        }
    }

    QSizeF m_size;

    bool m_repaintNeeded;

    MarbleGraphicsItem::CacheMode m_cacheMode;

    QPixmap m_pixmap;

    bool m_visibility;

    // The parent of the item
    MarbleGraphicsItem *const m_parent;
    // The set of children.
    QSet<MarbleGraphicsItem *> m_children;

    // The layout handling the positions of the children
    AbstractMarbleGraphicsLayout *m_layout;

    MarbleGraphicsItem *const m_marbleGraphicsItem;
};

}

#endif
