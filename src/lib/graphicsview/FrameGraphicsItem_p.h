//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_FRAMEGRAPHICSITEMPRIVATE_H
#define MARBLE_FRAMEGRAPHICSITEMPRIVATE_H

// Marble
#include "FrameGraphicsItem.h"

// Qt
#include<QtCore/QDebug>
#include<QtGui/QBrush>
#include<QtGui/QPixmapCache>

namespace Marble
{

class FrameGraphicsItemPrivate
{
 public:
    FrameGraphicsItemPrivate( FrameGraphicsItem *parent )
        : m_frame( FrameGraphicsItem::NoFrame ),
          m_contentSize( 0.0, 0.0 ),
          m_margin( 0.0 ),
          m_marginTop( 0.0 ),
          m_marginBottom( 0.0 ),
          m_marginLeft( 0.0 ),
          m_marginRight( 0.0 ),
          m_padding( 0.0 ),
          m_borderWidth( 1.0 ),
          m_borderBrush( QBrush( Qt::black ) ),
          m_borderStyle( Qt::SolidLine ),
          m_backgroundBrush( QBrush( QColor( 192, 192, 192, 192 ) ) ),
          m_parent( parent )
    {
        updateSize();
    }

    void updateSize()
    {
        qreal marginTop = ( m_marginTop == 0.0 ) ? m_margin : m_marginTop;
        qreal marginBottom = ( m_marginBottom == 0.0 ) ? m_margin : m_marginBottom;
        qreal marginLeft = ( m_marginLeft == 0.0 ) ? m_margin : m_marginLeft;
        qreal marginRight = ( m_marginRight == 0.0 ) ? m_margin : m_marginRight;

        QSizeF totalSize = m_contentSize;
        totalSize += QSizeF( marginLeft + marginRight, marginTop + marginBottom );
        totalSize += QSizeF( m_padding * 2, m_padding * 2 );

        m_parent->setSize( totalSize );
    }

    FrameGraphicsItem::FrameType m_frame;
    QSizeF m_contentSize;

    // Margin
    qreal m_margin;
    qreal m_marginTop;
    qreal m_marginBottom;
    qreal m_marginLeft;
    qreal m_marginRight;
    // Padding
    qreal m_padding;

    // Background/Border painting
    qreal m_borderWidth;
    QBrush m_borderBrush;
    Qt::PenStyle m_borderStyle;
    QBrush m_backgroundBrush;

    FrameGraphicsItem * const m_parent;
};

} // namespace Marble

#endif
