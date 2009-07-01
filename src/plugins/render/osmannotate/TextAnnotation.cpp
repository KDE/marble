//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "TextAnnotation.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "MarbleDirs.h"
#include "GeoWidgetBubble.h"
#include "TextEditor.h"
#include "AbstractProjection.h"

#include <QtCore/QDebug>
#include <QtGui/QPixmap>
#include <QtGui/QTextEdit>
#include <QtGui/QPushButton>
#include <QtCore/QVariant>

namespace Marble{

TextAnnotation::TextAnnotation()
{
   bubble = new GeoWidgetBubble();

   //FIXME decide who actually owns this widget and setup destruction
   //accordingly
   m_textEditor = new TextEditor();
   bubble->setGeoWidget( m_textEditor );
}

QRect TextAnnotation::screenBounding()
{
    //FIXME the first 2 should actually offset it
    return QRect(-5,-5,20,20);
}

void TextAnnotation::geoBounding(qreal angularResolution)
{
    //FIXME
    // Remove this method when the QRegion is actually done by the painter
    // it will be preduced in the GeoGraphicsItemPrivate during a paint event


    qreal lat, lon, alt, width, height;
    coordinate(lon, lat, alt);

    //problem with this approach is that the geoBounding is View
    //Dependant. This does not lend well to a good consistant model.
    width = screenBounding().width() * angularResolution;
    height = screenBounding().height() * angularResolution;
    QRectF bound( coordinate().longitude() - (width/2),
                 coordinate().latitude() - (height/2),
                 width, height) ;
    m_regions.clear();
    QPainterPath path;
    path.addRect(bound);
    m_regions.append(path);

}

void TextAnnotation::paint( GeoPainter *painter,
                            ViewportParams *viewport,
                            const QString& renderPos,
                            GeoSceneLayer * layer )
{
    qreal degPix = viewport->angularResolution() * RAD2DEG;

    painter->drawEllipse(coordinate(), screenBounding().width(), screenBounding().height(), isGeoProjected());
    //Would it not be useful to have a draw latlongbox?
//    painter->drawRect(geoBounding());
    qreal north, south, east, west;

    //don't need this if its done by the painter soon
    geoBounding(viewport->angularResolution());
    north = m_regions.at(0).boundingRect().top();
    south = m_regions.at(0).boundingRect().bottom();
    east = m_regions.at(0).boundingRect().right();
    west = m_regions.at(0).boundingRect().left();

    //would like a method to draw a QRegion ;)
    painter->drawRect(GeoDataCoordinates((west + east) /2 , (north + south) / 2, 0 ),
                      m_regions.at(0).boundingRect().width() * RAD2DEG,
                      m_regions.at(0).boundingRect().height() * RAD2DEG,true);

    painter->drawPoint( GeoDataCoordinates((west + east) /2 , (north + south) / 2, 0 ) );

    painter->drawPixmap( coordinate(), QPixmap( MarbleDirs::path( "bitmaps/annotation.png" ) )  );

    qreal x, y;
    bool hidden;
    bool visable = viewport->currentProjection()->screenCoordinates( coordinate(), viewport, x, y, hidden );

    if( visable && !hidden ) {
        bubble->moveTo( QPoint( x, y ) );
        bubble->paint( painter, viewport, renderPos, layer );
    } else {
        bubble->hide();
    }

    //FIXME This shouldn't really be a part of this method at all as each item should
    //be a part of the scene regardless if it has a parent or not!
    //Parent - Child relationship should only be used in the paint function to decide
    //if the coordinate of the object should be an offset of the parent or an actual
    //coordinate.
    QListIterator<TmpGraphicsItem*> it(getChildren());

    if( it.hasNext() ) {
        TmpGraphicsItem* p = it.next();
        p->paint(painter, viewport, renderPos, layer );
    }




}

bool TextAnnotation::isGeoProjected()
{
    return false;
}

QVariant TextAnnotation::itemChange(GeoGraphicsItemChange change, QVariant v )
{
    switch ( change ) {
        case TmpGraphicsItem::ItemSelectChange :
        if ( v.toBool() ) {
            //make the bubble visable
        } else {
            //hide the bubble
        }
        break;

        default:
        //do nothing
        break;
    }
    return TmpGraphicsItem::itemChange( change, v );
}

}
