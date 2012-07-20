//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009-2010 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_SCREENGRAPHICSITEMPRIVATE_H
#define MARBLE_SCREENGRAPHICSITEMPRIVATE_H

#include "MarbleGraphicsItem_p.h"
#include "MarbleDebug.h"

#include "GeoGraphicsItem.h"
#include "ScreenGraphicsItem.h"
#include "ViewportParams.h"

namespace Marble
{

class ScreenGraphicsItemPrivate : public MarbleGraphicsItemPrivate
{
 public:
    ScreenGraphicsItemPrivate( ScreenGraphicsItem *screenGraphicsItem,
                               MarbleGraphicsItem *parent )
        : MarbleGraphicsItemPrivate( screenGraphicsItem, parent ),
          m_position(),
          m_viewportSize(),
          m_floatItemMoving( false ),
          m_flags( 0 )
    {
    }

    virtual ~ScreenGraphicsItemPrivate()
    {
    }

    QList<QPointF> positions() const
    {
        QList<QPointF> list;

        list.append( positivePosition() );
        return list;
    }

    QPointF positivePosition() const
    {
        const QSizeF parentSize = m_parent ? m_parent->size() : m_viewportSize;
        if ( !parentSize.isValid() ) {
            mDebug() << "Invalid parent size";
            return m_position;
        }
        QPointF position;
        qreal x = m_position.x();
        qreal y = m_position.y();

        position.setX( ( x >= 0 ) ? x : parentSize.width() + x - m_size.width() );
        position.setY( ( y >= 0 ) ? y : parentSize.height() + y - m_size.height() );

        return position;
    }

    QList<QPointF> absolutePositions() const
    {
        if( m_parent == 0 ) {
            return positions();
        }

        QList<QPointF> parentPositions;
        ScreenGraphicsItem *screenItem = dynamic_cast<ScreenGraphicsItem*>( m_parent );

        if( screenItem ) {
            parentPositions = screenItem->absolutePositions();
        }
        else {
            GeoGraphicsItem *geoItem = dynamic_cast<GeoGraphicsItem*>( m_parent );
            if( geoItem ) {
                parentPositions = geoItem->positions();
            }
        }

        QPointF relativePosition = positivePosition();

        QList<QPointF> absolutePositions;
        foreach( QPointF point, parentPositions ) {
            absolutePositions.append( point + relativePosition );
        }

        return absolutePositions;
    }

    void setProjection( ViewportParams *viewport,
                        GeoPainter *painter )
    {
        Q_UNUSED( painter );

        // If we have no parent
        if( m_parent == 0 ) {
            // Saving the screen size needed for positions()
            m_viewportSize = viewport->size();
        }

        ScreenGraphicsItem *screenGraphicsItem
                = static_cast<ScreenGraphicsItem *>( m_marbleGraphicsItem );
        screenGraphicsItem->changeViewport( viewport );
    }

    bool isMovable() const
    {
        return ( m_flags & ScreenGraphicsItem::ItemIsMovable ) ? true : false;
    }

    QPointF             m_position;
    // The size of the parent, or if no parent exists the size of the viewport.
    QSizeF              m_viewportSize;

    QPoint              m_floatItemMoveStartPos;
    bool                m_floatItemMoving;

    ScreenGraphicsItem::GraphicsItemFlags m_flags;
};

}

#endif
