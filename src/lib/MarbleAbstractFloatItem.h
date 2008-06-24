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
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtCore/Qt>

#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtGui/QPainterPath>
#include <QtGui/QFont>

#include "MarbleLayerInterface.h"
#include "marble_export.h"


class MarbleAbstractFloatItemPrivate;


/**
 * @short The abstract class that creates an "empty" float item.
 *
 */

class MARBLE_EXPORT MarbleAbstractFloatItem : public MarbleLayerInterface
{
 public:
    explicit MarbleAbstractFloatItem( const QPointF &point = QPointF( 10.0, 10.0 ),
                                      const QSizeF &size = QSizeF( 150.0, 50.0 ) );
    virtual ~MarbleAbstractFloatItem();

    void    setPosition( const QPointF& position );
    QPointF position() const;
    QPointF positivePosition( const QRectF& viewPort ) const;

    void    setSize( const QSizeF& size );
    virtual QSizeF   size() const;

    void    setVisible( bool visible );
    bool    visible() const;


    QPen pen() const;
    void setPen( const QPen &pen );

    QBrush background() const;
    void setBackground( const QBrush &background );

    QRectF contentRect() const;
    QRectF renderedRect() const;

    virtual QPainterPath backgroundShape() const;
    void renderBackground( QPainter* );

    double border() const;
    void setBorder( double width );

    QBrush borderBrush() const;
    void setBorderBrush( const QBrush &brush );

    Qt::PenStyle borderStyle () const; 
    void setBorderStyle( Qt::PenStyle );

    QFont font() const;
    void setFont( const QFont &font );

    double margin() const; 
    void setMargin( double margin );

    double marginTop() const; 
    void setMarginTop( double marginTop );

    double marginBottom() const; 
    void setMarginBottom( double marginBottom );

    double marginLeft() const; 
    void setMarginLeft( double marginLeft );

    double marginRight() const; 
    void setMarginRight( double marginRight );

    double padding () const;
    void setPadding( double width );

    virtual bool needsUpdate( ViewportParams *viewport );

    bool pixmapCacheEnabled() const;
    void setPixmapCacheEnabled( bool enabled );

    bool render( GeoPainter *painter, ViewportParams *viewport, const QString& pos = "FLOAT_ITEM", GeoSceneLayer * layer = 0 );

    virtual bool renderFloatItem( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer * layer = 0 );

    virtual bool renderOnMap( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0 );

    virtual QString renderPolicy() const;

    virtual QStringList renderPosition() const;

 private:
    Q_DISABLE_COPY( MarbleAbstractFloatItem )
    MarbleAbstractFloatItemPrivate  * const d;
};

#endif // MARBLE_ABSTRACT_FLOATITEM_H
