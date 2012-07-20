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
#include "GeoPainter.h"
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
        : m_removeCachedPixmap( false ),
          m_cacheMode( MarbleGraphicsItem::NoCache ),
          m_visibility( true ),
          m_parent( parent ),
          m_children(),
          m_layout( 0 ),
          m_marbleGraphicsItem( marbleGraphicsItem )
    {
        if ( m_parent ) {
            m_parent->p()->addChild( m_marbleGraphicsItem );
            setParentSize( m_parent->size() );
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

    virtual QList<QPointF> absolutePositions() const
    {
        return positions();
    }

    void ensureValidCacheKey()
    {
#if QT_VERSION < 0x040600
        if( m_cacheKey.isNull() ) {
            static unsigned int key = 0;
            m_cacheKey = QString( "MarbleGraphicsItem:" ) + QString::number( key );
            key++;
        }
#endif
    }

    virtual void setProjection( ViewportParams *viewport,
                                GeoPainter *painter )
    {
        Q_UNUSED( viewport );
        Q_UNUSED( painter );
    }

    virtual void setParentSize( QSizeF size )
    {
        Q_UNUSED( size );
    }

    void updateLabelPositions()
    {
        // This has to be done recursively because we need a correct size from all children.
        foreach ( MarbleGraphicsItem *item, m_children ) {
            item->p()->updateLabelPositions();
        }

        // Adjust positions
        if ( m_layout ) {
            m_layout->updatePositions( m_marbleGraphicsItem );
        }
    }

    QSizeF m_size;

    bool m_removeCachedPixmap;

    MarbleGraphicsItem::CacheMode m_cacheMode;

#if QT_VERSION < 0x040600
    QString m_cacheKey;
#else
    QPixmapCache::Key m_cacheKey;
#endif

    bool m_visibility;

    // The parent of the item
    MarbleGraphicsItem *const m_parent;
    // The set of children.
    QSet<MarbleGraphicsItem *> m_children;

    // The layout handling the positions of the children
    AbstractMarbleGraphicsLayout *m_layout;
    
    QString m_toolTip;

    MarbleGraphicsItem *const m_marbleGraphicsItem;
};

}

#endif
