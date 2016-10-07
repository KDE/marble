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
#include "ScreenGraphicsItem_p.h"

// Qt
#include<QDebug>
#include<QBrush>

namespace Marble
{

class FrameGraphicsItemPrivate : public ScreenGraphicsItemPrivate
{
 public:
    FrameGraphicsItemPrivate(FrameGraphicsItem *frameGraphicsItem,
                             MarbleGraphicsItem *parent)
        : ScreenGraphicsItemPrivate(frameGraphicsItem, parent),
          m_frame(FrameGraphicsItem::NoFrame),
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
          m_backgroundBrush(QBrush(QColor(192, 192, 192, 192)))
    {
    }

    void updateSize()
    {
        qreal const border2 = 0.5 * m_borderWidth;
        qreal marginTop = qMax( border2, ( m_marginTop == 0.0 ) ? m_margin : m_marginTop );
        qreal marginBottom = qMax( border2, ( m_marginBottom == 0.0 ) ? m_margin : m_marginBottom );
        qreal marginLeft = qMax( border2, ( m_marginLeft == 0.0 ) ? m_margin : m_marginLeft );
        qreal marginRight = qMax( border2, ( m_marginRight == 0.0 ) ? m_margin : m_marginRight );

        QSizeF totalSize = m_contentSize;
        totalSize += QSizeF( marginLeft + marginRight, marginTop + marginBottom );
        totalSize += QSizeF( m_padding * 2, m_padding * 2 );

        m_marbleGraphicsItem->setSize(totalSize);
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
};

} // namespace Marble

#endif
