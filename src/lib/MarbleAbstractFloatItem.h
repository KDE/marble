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

#include <QtGui/QBrush>
#include <QtGui/QPainterPath>


/**
 * @short The abstract class that creates an "empty" float item.
 *
 */

class MarbleAbstractFloatItem : MarbleLayerInterface
{
 public:
    virtual ~MarbleAbstractFloatItem(){}

    void    setPosition( const QPointF& position );
    QPointF position() const;

    void    setSize( const QSize& size );
    virtual QSize   size() const;

    void    setVisible( bool visible );
    bool    visible const;


    QPen pen() const;
    void setPen( const QPen &pen );


    virtual QPainterPath backgroundShape() const;

    double border() const;
    void setBorder( double width );

    QBrush borderBrush() const;
    void setBorderBrush( const QBrush &brush );

    Qt::PenStyle borderStyle () const; 
    void setBorderStyle( Qt::PenStyle );

    double margin() const; 
    void setMargin( double margin );

    double padding () const;
    void setPadding( double width );


    bool pixmapCacheEnabled() const;
    void setPixmapCacheEnabled( bool enabled );

    virtual QString renderPolicy() const { return "ALWAYS"; }

    virtual QString renderPosition() const { return "ALWAYS_ON_TOP"; }

 private:
    MarbleAbstractFloatItemPrivate  * const d;
};

#endif // MARBLE_ABSTRACT_FLOATITEM_H
