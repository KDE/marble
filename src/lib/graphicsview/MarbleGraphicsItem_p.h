//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009-2010 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_MARBLEGRAPHICSITEMPRIVATE_H
#define MARBLE_MARBLEGRAPHICSITEMPRIVATE_H

// Marble
#include "AbstractMarbleGraphicsLayout.h"
#include "MarbleGraphicsItem.h"

// Qt
#include<QtCore/QDebug>
#include<QtCore/QList>
#include<QtCore/QSet>
#include<QtCore/QSize>
#include<QtCore/QSizeF>
#include<QtCore/QRect>
#include<QtGui/QPixmapCache>

namespace Marble
{

class MarbleGraphicsItemPrivate
{
 public:
    explicit MarbleGraphicsItemPrivate( MarbleGraphicsItem *marbleGraphicsItem,
                                        MarbleGraphicsItem *parent = 0 )
        : m_repaintNeeded( true ),
          m_cacheMode( MarbleGraphicsItem::NoCache ),
          m_visibility( true ),
          m_parent( parent ),
          m_children(),
          m_layout( 0 ),
          m_marbleGraphicsItem( marbleGraphicsItem )
    {
        if ( m_parent ) {
            m_parent->p()->addChild( m_marbleGraphicsItem );
        }
    }

    virtual ~MarbleGraphicsItemPrivate()
    {
        // Remove from parent
        if ( m_parent ) {
            m_parent->p()->removeChild( m_marbleGraphicsItem );
        }

        // Delete all children
        qDeleteAll( m_children.toList() ); // delete using a copy, since children may invalidate m_children's iterator

        // Delete Layout
        delete m_layout;
    }

    void addChild( MarbleGraphicsItem *child )
    {
        m_children.insert( child );
    }

    void removeChild( MarbleGraphicsItem *child )
    {
        m_children.remove( child );
    }

    virtual QList<QPointF> positions() const = 0;

    virtual QList<QPointF> absolutePositions() const = 0;

    /**
     * @brief Used to get the set of screen bounding rects
     */
    QList<QRectF> boundingRects() const;

    virtual void setProjection( const ViewportParams *viewport ) = 0;

    void updateChildPositions()
    {
        // This has to be done recursively because we need a correct size from all children.
        foreach ( MarbleGraphicsItem *item, m_children ) {
            item->p()->updateChildPositions();
        }

        // Adjust positions
        if ( m_layout ) {
            m_layout->updatePositions( m_marbleGraphicsItem );
        }
    }

    QSizeF m_size;

    bool m_repaintNeeded;

    MarbleGraphicsItem::CacheMode m_cacheMode;

    QPixmapCache::Key m_cacheKey;

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
