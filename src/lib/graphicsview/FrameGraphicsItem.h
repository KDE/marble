//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_FRAMEGRAPHICSITEM_H
#define MARBLE_FRAMEGRAPHICSITEM_H

// Marble
#include "marble_export.h"
#include "ScreenGraphicsItem.h"
#include <QtGui/QPainterPath>
class QBrush;

namespace Marble
{

class FrameGraphicsItemPrivate;

class MARBLE_EXPORT FrameGraphicsItem : public ScreenGraphicsItem
{
 public:
    enum FrameType {
        NoFrame,
        RectFrame,
        RoundedRectFrame
    };

    explicit FrameGraphicsItem( MarbleGraphicsItem *parent = 0 );

    virtual ~FrameGraphicsItem();

    /**
     * Returns the type of the frame.
     */
    FrameType frame() const;

    /**
     * Sets the type of the Frame. Standard is NoFrame.
     */
    void setFrame( FrameType type );

    /**
     * Returns the margin of the item. This is used for all margins with the value 0.0.
     * The padding is the space outside the painted space.
     */
    qreal margin() const;

    /**
     * Sets the margin of the item. This is used for all margins with the value 0.0.
     */
    void setMargin( qreal margin );

    /**
     * Returns the top margin of the item.
     */
    qreal marginTop() const;

    /**
     * Set the top margin of the item.
     */
    void setMarginTop( qreal marginTop );

    /**
     * Returns the bottom margin of the item.
     */
    qreal marginBottom() const;

    /**
     * Set the bottom margin of the item.
     */
    void setMarginBottom( qreal marginBottom );

    /**
     * Returns the left margin of the item.
     */
    qreal marginLeft() const;

    /**
     * Set the left margin of the item.
     */
    void setMarginLeft( qreal marginLeft );

    /**
     * Returns the right margin of the item.
     */
    qreal marginRight() const;

    /**
     * Set the right margin of the item.
     */
    void setMarginRight( qreal marginRight );

    /**
     * Returns the border width of the item.
     */
    qreal borderWidth() const;

    /**
     * Set the border width of the item.
     */
    void setBorderWidth( qreal width );

    /**
     * Returns the padding of the item.
     * The padding is the empty space inside the border.
     */
    qreal padding() const;

    /**
     * Set the padding of the item.
     */
    void setPadding( qreal width );

    /**
     * Returns the brush of the border.
     */
    QBrush borderBrush() const;

    /**
     * Change the brush of the border.
     */
    void setBorderBrush( const QBrush &brush );

    /**
     * Returns the style of the border.
     */
    Qt::PenStyle borderStyle () const;

    /**
     * Change the style of the border.
     */
    void setBorderStyle( Qt::PenStyle style );

    /**
     * Returns the background brush of the item.
     */
    QBrush background() const;

    /**
     * Changes the background brush of the item.
     */
    void setBackground( const QBrush &background );

    QRectF contentRect( const QPointF& position ) const;
    QRectF paintedRect( const QPointF& position = QPointF( 0.0, 0.0 ) ) const;

    QRectF contentRect() const;
    QSizeF contentSize() const;

    /**
     * Sets the size of the content of the item.
     * @p size is the size required for contents.
     */
    void setContentSize( const QSizeF& size );

 protected:
    /**
     * Returns the shape of the background.
     */
    virtual QPainterPath backgroundShape() const;

    /**
     * This function won't be reimplemented in most cases.
     */
    virtual void paint( GeoPainter *painter, ViewportParams *viewport,
                        const QString& renderPos, GeoSceneLayer * layer = 0 );

    /**
     * Here the items paint their content.
     */
    virtual void paintContent( GeoPainter *painter, ViewportParams *viewport,
                               const QString& renderPos, GeoSceneLayer * layer = 0 );

    /**
     * Paints the background. This function won't be reimplemented in most cases.
     */
    virtual void paintBackground( GeoPainter *painter );

 private:
    Q_DISABLE_COPY( FrameGraphicsItem )

    FrameGraphicsItemPrivate * const d;
};

} // namespace Marble

#endif
