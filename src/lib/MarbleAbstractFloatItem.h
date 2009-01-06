//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//


#ifndef MARBLE_ABSTRACT_FLOATITEM_H
#define MARBLE_ABSTRACT_FLOATITEM_H

#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtCore/QSizeF>
#include <QtCore/QString>
#include <QtCore/Qt>

#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtGui/QPainterPath>
#include <QtGui/QFont>

#include "MarbleRenderPlugin.h"
#include "marble_export.h"

class QAction;
class QPainter;

namespace Marble
{

class MarbleAbstractFloatItemPrivate;


/**
 * @short The abstract class that creates an "empty" float item.
 *
 */

class MARBLE_EXPORT MarbleAbstractFloatItem : public MarbleRenderPlugin
{
    Q_OBJECT

 public:
    explicit MarbleAbstractFloatItem( const QPointF &point = QPointF( 10.0, 10.0 ),
                                      const QSizeF &size = QSizeF( 150.0, 50.0 ) );
    virtual ~MarbleAbstractFloatItem();

    /**
     * @brief Set the position of the float item
     * @param pos Position
     *
     * Positive x-coordinates are counted left-aligned from the left map border.
     * Negative x-coordinates are counted right-aligned from the right map border.
     *
     * Positive y-coordinates are counted top-aligned from the top map border.
     * Negative y-coordinates are counted right-aligned from the bottom map border.
     */
    void    setPosition( const QPointF& position );

    /**
     * @brief Return the position of the float item
     *
     * Positive x-coordinates are counted left-aligned from the left map border.
     * Negative x-coordinates are counted right-aligned from the right map border.
     *
     * Positive y-coordinates are counted top-aligned from the top map border.
     * Negative y-coordinates are counted right-aligned from the bottom map border.
     */
    QPointF position() const;

    /**
     * @brief Return the positive position of the float item
     * 
     * All coordinates are counted positive and depend on the current viewport.
     *
     * Positive x-coordinates are counted left-aligned from the left map border.
     * Positive y-coordinates are counted top-aligned from the top map border.
     */
    QPointF positivePosition( const QRectF& viewPort ) const;

    void    setSize( const QSizeF& size );
    virtual QSizeF   size() const;


    QPen pen() const;
    void setPen( const QPen &pen );

    QBrush background() const;
    void setBackground( const QBrush &background );

    QRectF contentRect() const;
    QRectF renderedRect() const;

    virtual QPainterPath backgroundShape() const;
    void renderBackground( QPainter* );

    qreal border() const;
    void setBorder( qreal width );

    QBrush borderBrush() const;
    void setBorderBrush( const QBrush &brush );

    Qt::PenStyle borderStyle () const; 
    void setBorderStyle( Qt::PenStyle );

    QFont font() const;
    void setFont( const QFont &font );

    qreal margin() const; 
    void setMargin( qreal margin );

    qreal marginTop() const; 
    void setMarginTop( qreal marginTop );

    qreal marginBottom() const; 
    void setMarginBottom( qreal marginBottom );

    qreal marginLeft() const; 
    void setMarginLeft( qreal marginLeft );

    qreal marginRight() const; 
    void setMarginRight( qreal marginRight );

    qreal padding () const;
    void setPadding( qreal width );

    bool positionLocked() const;
    void setPositionLocked( bool enabled );

    virtual bool needsUpdate( ViewportParams *viewport );

    bool pixmapCacheEnabled() const;
    void setPixmapCacheEnabled( bool enabled );

    bool render( GeoPainter *painter, ViewportParams *viewport, const QString& pos = "FLOAT_ITEM", GeoSceneLayer * layer = 0 );

    virtual bool renderFloatItem( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer * layer = 0 );

    virtual bool renderOnMap( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0 );

    virtual QString renderPolicy() const;

    virtual QStringList renderPosition() const;

 protected:
    bool eventFilter( QObject *, QEvent * );

 private:
    Q_DISABLE_COPY( MarbleAbstractFloatItem )
    MarbleAbstractFloatItemPrivate  * const d;
};

}

#endif // MARBLE_ABSTRACT_FLOATITEM_H
