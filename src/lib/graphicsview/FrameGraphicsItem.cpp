//
// This file is part of the Marble Desktop Globe.
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
#include "GeoPainter.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QSizeF>

using namespace Marble;

FrameGraphicsItem::FrameGraphicsItem( MarbleGraphicsItem *parent )
        : ScreenGraphicsItem( parent ),
          d( new FrameGraphicsItemPrivate() )
{
}

FrameGraphicsItem::FrameGraphicsItem( const QPointF& position, const QSizeF& size,
                      MarbleGraphicsItem *parent )
        : ScreenGraphicsItem( position, size, parent ),
          d( new FrameGraphicsItemPrivate() )
{
}

FrameGraphicsItem::~FrameGraphicsItem()
{
    delete d;
}

FrameGraphicsItem::FrameType FrameGraphicsItem::frame()
{
    return d->m_frame;
}

void FrameGraphicsItem::setFrame( FrameType type ) {
    d->m_frame = type;
    if ( type == RectFrame && padding() < 2 ) {
        setPadding( 2 );
    }
}

qreal FrameGraphicsItem::margin() const
{
    return d->m_margin;
}

void FrameGraphicsItem::setMargin( qreal margin )
{
    d->m_margin = margin;
    update();
}

qreal FrameGraphicsItem::marginTop() const
{
    return d->m_marginTop;
}

void FrameGraphicsItem::setMarginTop( qreal marginTop )
{
    d->m_marginTop = marginTop;
    update();
}

qreal FrameGraphicsItem::marginBottom() const
{
    return d->m_marginBottom;
}

void FrameGraphicsItem::setMarginBottom( qreal marginBottom )
{
    d->m_marginBottom = marginBottom;
    update();
}

qreal FrameGraphicsItem::marginLeft() const
{
    return d->m_marginLeft;
}

void FrameGraphicsItem::setMarginLeft( qreal marginLeft )
{
    d->m_marginLeft = marginLeft;
    update();
}

qreal FrameGraphicsItem::marginRight() const
{
    return d->m_marginRight;
}

void FrameGraphicsItem::setMarginRight( qreal marginRight )
{
    d->m_marginRight = marginRight;
    update();
}

qreal FrameGraphicsItem::borderWidth() const
{
    return d->m_borderWidth;
}

void FrameGraphicsItem::setBorderWidth( qreal width )
{
    d->m_borderWidth = width;
    update();
}

qreal FrameGraphicsItem::padding() const
{
    return d->m_padding;
}

void FrameGraphicsItem::setPadding( qreal width )
{
    d->m_padding = width;
}

QBrush FrameGraphicsItem::borderBrush() const
{
    return d->m_borderBrush;
}

void FrameGraphicsItem::setBorderBrush( const QBrush &brush )
{
    d->m_borderBrush = brush;
    update();
}

Qt::PenStyle FrameGraphicsItem::borderStyle () const
{
    return d->m_borderStyle;
}

void FrameGraphicsItem::setBorderStyle( Qt::PenStyle style )
{
    d->m_borderStyle = style;
    update();
}

QBrush FrameGraphicsItem::background() const
{
    return d->m_backgroundBrush;
}

void FrameGraphicsItem::setBackground( const QBrush &background )
{
    d->m_backgroundBrush = background;
    update();
}

QRectF FrameGraphicsItem::contentRect( const QPointF& position ) const
{
    qreal marginTop = ( d->m_marginTop == 0.0 ) ? d->m_margin : d->m_marginTop;
    qreal marginBottom = ( d->m_marginBottom == 0.0 ) ? d->m_margin : d->m_marginBottom;
    qreal marginLeft = ( d->m_marginLeft == 0.0 ) ? d->m_margin : d->m_marginLeft;
    qreal marginRight = ( d->m_marginRight == 0.0 ) ? d->m_margin : d->m_marginRight;

    QSizeF size = this->size();

    QRectF contentRect = QRectF( position.x() + marginLeft + d->m_padding,
                                 position.y() + marginTop + d->m_padding,
                                 size.width() - ( marginLeft + marginRight + 2.0 * d->m_padding ),
                                 size.height() - ( marginTop + marginBottom + 2.0 * d->m_padding ) );

    return contentRect;
}

QRectF FrameGraphicsItem::contentRect() const {
    return contentRect( QPointF( 0.0, 0.0 ) );
}

QSizeF FrameGraphicsItem::contentSize() const {
    return contentRect().size();
}

QRectF FrameGraphicsItem::paintedRect( const QPointF& position ) const
{
    qreal marginTop = ( d->m_marginTop == 0.0 ) ? d->m_margin : d->m_marginTop;
    qreal marginBottom = ( d->m_marginBottom == 0.0 ) ? d->m_margin : d->m_marginBottom;
    qreal marginLeft = ( d->m_marginLeft == 0.0 ) ? d->m_margin : d->m_marginLeft;
    qreal marginRight = ( d->m_marginRight == 0.0 ) ? d->m_margin : d->m_marginRight;

    QSizeF size = this->size();

    QRectF paintedRect = QRectF( position.x() + marginLeft,
                                 position.y() + marginTop,
                                 size.width() - ( marginLeft + marginRight ),
                                 size.height() - ( marginTop + marginBottom ) );
    return paintedRect;
}

void FrameGraphicsItem::setContentSize( const QSizeF& size )
{
    qreal marginTop = ( d->m_marginTop == 0.0 ) ? d->m_margin : d->m_marginTop;
    qreal marginBottom = ( d->m_marginBottom == 0.0 ) ? d->m_margin : d->m_marginBottom;
    qreal marginLeft = ( d->m_marginLeft == 0.0 ) ? d->m_margin : d->m_marginLeft;
    qreal marginRight = ( d->m_marginRight == 0.0 ) ? d->m_margin : d->m_marginRight;

    QSizeF totalSize = size;
    totalSize += QSizeF( marginLeft + marginRight, marginTop + marginBottom );
    totalSize += QSizeF( d->m_padding * 2, d->m_padding * 2 );

    setSize( totalSize );
}

QPainterPath FrameGraphicsItem::backgroundShape() const
{
    if ( d->m_frame == RectFrame ) {
        QRectF renderedRect = paintedRect( QPointF( 0.0, 0.0 ) );
        QPainterPath path;
        path.addRect( QRectF( 0.0, 0.0, renderedRect.size().width(), renderedRect.size().height() ) );
        return path;
    }
    else {
        return QPainterPath();
    }
}

void FrameGraphicsItem::paintBackground( GeoPainter *painter )
{
    painter->save();

    painter->setPen( QPen( d->m_borderBrush, d->m_borderWidth, d->m_borderStyle ) );
    painter->setBrush( d->m_backgroundBrush );
    painter->drawPath( backgroundShape() );

    painter->restore();
}

void FrameGraphicsItem::paint( GeoPainter *painter, ViewportParams *viewport,
            const QString& renderPos, GeoSceneLayer * layer )
{
    painter->save();
    painter->translate( paintedRect( QPointF( 0.0, 0.0 ) ).topLeft() );
    paintBackground( painter );
    painter->translate( padding(), padding() );
    paintContent( painter, viewport, renderPos, layer );
    painter->restore();
}

void FrameGraphicsItem::paintContent( GeoPainter *painter, ViewportParams *viewport,
                   const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( painter )
    Q_UNUSED( viewport )
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )
}
