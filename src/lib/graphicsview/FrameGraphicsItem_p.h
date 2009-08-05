//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef FRAMEGRAPHICSITEMPRIVATE_H
#define FRAMEGRAPHICSITEMPRIVATE_H

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
    FrameGraphicsItemPrivate()
        : m_frame( FrameGraphicsItem::NoFrame ),
          m_margin( 0.0 ),
          m_marginTop( 0.0 ),
          m_marginBottom( 0.0 ),
          m_marginLeft( 0.0 ),
          m_marginRight( 0.0 ),
          m_padding( 0.0 ),
          m_borderWidth( 1.0 ),
          m_borderBrush( QBrush( Qt::black ) ),
          m_borderStyle( Qt::SolidLine ),
          m_backgroundBrush( QBrush( QColor( 192, 192, 192, 192 ) ) )
    {
    }

    FrameGraphicsItem::FrameType m_frame;

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

#endif // FRAMEGRAPHICSITEMPRIVATE_H
