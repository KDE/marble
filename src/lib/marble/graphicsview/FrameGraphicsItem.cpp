//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "FrameGraphicsItem.h"
#include "FrameGraphicsItem_p.h"

// Marble
#include "MarbleDebug.h"

// Qt
#include <QSizeF>
#include <QPainter>
#include <QPixmapCache>
#include <QMargins>
#include <qdrawutil.h>

using namespace Marble;

FrameGraphicsItem::FrameGraphicsItem( MarbleGraphicsItem *parent )
    : ScreenGraphicsItem(new FrameGraphicsItemPrivate(this, parent))
{
    Q_D(FrameGraphicsItem);
    d->updateSize();
}

FrameGraphicsItem::FrameGraphicsItem(FrameGraphicsItemPrivate *dd)
    : ScreenGraphicsItem(dd)
{
    Q_D(FrameGraphicsItem);
    d->updateSize();
}

FrameGraphicsItem::~FrameGraphicsItem()
{
}

FrameGraphicsItem::FrameType FrameGraphicsItem::frame() const
{
    Q_D(const FrameGraphicsItem);
    return d->m_frame;
}

void FrameGraphicsItem::setFrame( FrameType type )
{
    Q_D(FrameGraphicsItem);
    d->m_frame = type;
    setPadding( padding() );
}

qreal FrameGraphicsItem::margin() const
{
    Q_D(const FrameGraphicsItem);
    return d->m_margin;
}

void FrameGraphicsItem::setMargin( qreal margin )
{
    Q_D(FrameGraphicsItem);
    d->m_margin = margin;
    d->updateSize();
    update();
}

qreal FrameGraphicsItem::marginTop() const
{
    Q_D(const FrameGraphicsItem);
    return d->m_marginTop;
}

void FrameGraphicsItem::setMarginTop( qreal marginTop )
{
    Q_D(FrameGraphicsItem);
    d->m_marginTop = marginTop;
    d->updateSize();
    update();
}

qreal FrameGraphicsItem::marginBottom() const
{
    Q_D(const FrameGraphicsItem);
    return d->m_marginBottom;
}

void FrameGraphicsItem::setMarginBottom( qreal marginBottom )
{
    Q_D(FrameGraphicsItem);
    d->m_marginBottom = marginBottom;
    d->updateSize();
    update();
}

qreal FrameGraphicsItem::marginLeft() const
{
    Q_D(const FrameGraphicsItem);
    return d->m_marginLeft;
}

void FrameGraphicsItem::setMarginLeft( qreal marginLeft )
{
    Q_D(FrameGraphicsItem);
    d->m_marginLeft = marginLeft;
    d->updateSize();
    update();
}

qreal FrameGraphicsItem::marginRight() const
{
    Q_D(const FrameGraphicsItem);
    return d->m_marginRight;
}

void FrameGraphicsItem::setMarginRight( qreal marginRight )
{
    Q_D(FrameGraphicsItem);
    d->m_marginRight = marginRight;
    d->updateSize();
    update();
}

qreal FrameGraphicsItem::borderWidth() const
{
    Q_D(const FrameGraphicsItem);
    return d->m_borderWidth;
}

void FrameGraphicsItem::setBorderWidth( qreal width )
{
    Q_D(FrameGraphicsItem);
    d->m_borderWidth = width;
    d->updateSize();
    update();
}

qreal FrameGraphicsItem::padding() const
{
    Q_D(const FrameGraphicsItem);
    return d->m_padding;
}

void FrameGraphicsItem::setPadding( qreal width )
{
    Q_D(FrameGraphicsItem);
    if ( width >= 0 ) {
        d->m_padding = width;
        d->updateSize();
    }
}

QBrush FrameGraphicsItem::borderBrush() const
{
    Q_D(const FrameGraphicsItem);
    return d->m_borderBrush;
}

void FrameGraphicsItem::setBorderBrush( const QBrush &brush )
{
    Q_D(FrameGraphicsItem);
    d->m_borderBrush = brush;
    update();
}

Qt::PenStyle FrameGraphicsItem::borderStyle () const
{
    Q_D(const FrameGraphicsItem);
    return d->m_borderStyle;
}

void FrameGraphicsItem::setBorderStyle( Qt::PenStyle style )
{
    Q_D(FrameGraphicsItem);
    d->m_borderStyle = style;
    update();
}

QBrush FrameGraphicsItem::background() const
{
    Q_D(const FrameGraphicsItem);
    return d->m_backgroundBrush;
}

void FrameGraphicsItem::setBackground( const QBrush &background )
{
    Q_D(FrameGraphicsItem);
    d->m_backgroundBrush = background;
    update();
}

QRectF FrameGraphicsItem::contentRect() const
{
    Q_D(const FrameGraphicsItem);
    qreal marginTop = ( d->m_marginTop == 0.0 ) ? d->m_margin : d->m_marginTop;
    qreal marginLeft = ( d->m_marginLeft == 0.0 ) ? d->m_margin : d->m_marginLeft;

    QRectF contentRect = QRectF( marginLeft + d->m_padding,
                                 marginTop + d->m_padding,
                                 d->m_contentSize.width(),
                                 d->m_contentSize.height() );

    return contentRect;
}

QSizeF FrameGraphicsItem::contentSize() const
{
    Q_D(const FrameGraphicsItem);
    return d->m_contentSize;
}

QRectF FrameGraphicsItem::paintedRect() const
{
    Q_D(const FrameGraphicsItem);
    qreal marginTop = ( d->m_marginTop == 0.0 ) ? d->m_margin : d->m_marginTop;
    qreal marginBottom = ( d->m_marginBottom == 0.0 ) ? d->m_margin : d->m_marginBottom;
    qreal marginLeft = ( d->m_marginLeft == 0.0 ) ? d->m_margin : d->m_marginLeft;
    qreal marginRight = ( d->m_marginRight == 0.0 ) ? d->m_margin : d->m_marginRight;

    QSizeF size = this->size();

    QRectF paintedRect = QRectF( marginLeft, marginTop,
                                 size.width() - ( marginLeft + marginRight ),
                                 size.height() - ( marginTop + marginBottom ) );
    return paintedRect;
}

void FrameGraphicsItem::setContentSize( const QSizeF& size )
{
    Q_D(FrameGraphicsItem);
    d->m_contentSize = size;
    d->updateSize();
}

QPainterPath FrameGraphicsItem::backgroundShape() const
{
    Q_D(const FrameGraphicsItem);
    QPainterPath path;
    if ( d->m_frame == RectFrame || d->m_frame == ShadowFrame ) {
        QRectF renderedRect = paintedRect();
        path.addRect( QRectF( 0.0, 0.0, renderedRect.size().width(), renderedRect.size().height() ) );
    }
    else if ( d->m_frame == RoundedRectFrame ) {
        QSizeF paintedSize = paintedRect().size();
        path.addRoundedRect( QRectF( 0.0, 0.0, paintedSize.width() - 1, paintedSize.height() - 1 ),
                             6, 6 );
    }
    return path;
}

void FrameGraphicsItem::paintBackground( QPainter *painter )
{
    Q_D(FrameGraphicsItem);
    painter->save();
    painter->setPen( QPen( d->m_borderBrush, d->m_borderWidth, d->m_borderStyle ) );
    painter->setBrush( d->m_backgroundBrush );
    painter->drawPath( backgroundShape() );

    painter->restore();
}

void FrameGraphicsItem::paint( QPainter *painter )
{
    Q_D(FrameGraphicsItem);
    painter->save();

    // Needs to be done here cause we don't want the margin translation
    if ( frame() == ShadowFrame )
    {
        QPixmap shadow;
        if ( !QPixmapCache::find( "marble/frames/shadowframe.png", shadow ) ) {
            shadow = QPixmap(QStringLiteral(":/marble/frames/shadowframe.png"));
            QPixmapCache::insert( "marble/frames/shadowframe.png", shadow );
        }
        qDrawBorderPixmap( painter, QRect( QPoint( 0, 0 ), size().toSize() ),
                           QMargins( 10, 10, 10, 10 ), shadow );
    }

    painter->translate( paintedRect().topLeft() );
    paintBackground( painter );
    painter->translate( d->m_padding, d->m_padding );
    paintContent( painter );
    painter->restore();
}

void FrameGraphicsItem::paintContent( QPainter *painter )
{
    Q_UNUSED( painter )
}
