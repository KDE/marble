//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef SCREENGRAPHICSITEMPRIVATE_H
#define SCREENGRAPHICSITEMPRIVATE_H

#include "MarbleGraphicsItem_p.h"

#include "ScreenGraphicsItem.h"
#include "ViewportParams.h"

#include <QtCore/QDebug>

namespace Marble {

class ScreenGraphicsItemPrivate : public MarbleGraphicsItemPrivate {
 public:
    ScreenGraphicsItemPrivate( ScreenGraphicsItem *screenGraphicsItem,
                               MarbleGraphicsItem *parent )
        : MarbleGraphicsItemPrivate( screenGraphicsItem, parent ),
          m_position(),
          m_parentSize(),
          m_positionLocked( false ),
          m_floatItemMoving( false ),
          m_flags( 0 )
    {
    }
    
    virtual ~ScreenGraphicsItemPrivate() {
    }
     
    QList<QPointF> positions() {
        QList<QPointF> list;

        list.append( positivePosition() );
        return list;
    }

    QPointF positivePosition() {
        if ( !m_parentSize.isValid() ) {
            qDebug() << "Invalid screen size";
        }
        QPointF position;
        qreal x = m_position.x();
        qreal y = m_position.y();

        position.setX( ( x >= 0 ) ? x : m_parentSize.width() + x - m_size.width() );
        position.setY( ( y >= 0 ) ? y : m_parentSize.height() + y - m_size.height() );

        return position;
    }
    
    void setProjection( AbstractProjection *projection, ViewportParams *viewport ) {
        Q_UNUSED( projection );

        // If we have no parent
        if( m_parent == 0 ) {
            // Saving the screen size needed for positions()
            m_parentSize = viewport->size();
        }
        else {
            m_parentSize = m_parent->size();
        }

        ScreenGraphicsItem *screenGraphicsItem
                = static_cast<ScreenGraphicsItem *>( m_marbleGraphicsItem );
        screenGraphicsItem->changeViewport( viewport );
    }

    bool isMovable() {
        return ( m_flags & ScreenGraphicsItem::ItemIsMovable ) ? true : false;
    }

    QPointF             m_position;
    // The size of the parent, or if no parent exists the size of the viewport.
    QSizeF              m_parentSize;

    bool                m_positionLocked;
    QPoint              m_floatItemMoveStartPos;
    bool                m_floatItemMoving;

    ScreenGraphicsItem::GraphicsItemFlags m_flags;
};

}

#endif // SCREENGRAPHICSITEMPRIVATE_H
