//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "PlacemarkTextAnnotation.h"


#include "AbstractProjection.h"
#include "GeoDataPlacemark.h"
#include "GeoDocument.h"
#include "GeoPainter.h"
#include "GeoWidgetBubble.h"
#include "ViewportParams.h"
#include "MarbleDirs.h"
#include "TextEditor.h"
#include "MarbleDebug.h"

#include <QtGui/QPixmap>
#include <QtGui/QTextEdit>
#include <QtGui/QPushButton>
#include <QtCore/QVariant>

namespace Marble
{

PlacemarkTextAnnotation::PlacemarkTextAnnotation()
        : TextAnnotation(), TmpGraphicsItem()
{
    bubble = new GeoWidgetBubble();

    //FIXME decide who actually owns this widget and setup destruction
    //accordingly
    m_textEditor = new TextEditor();
    bubble->setGeoWidget( m_textEditor );
}

QRect PlacemarkTextAnnotation::screenBounding() const
{
    //FIXME the first 2 should actually offset it
    return QRect(-5,-5,20,20);
}

void PlacemarkTextAnnotation::paint( GeoPainter *painter,
                            ViewportParams *viewport,
                            const QString& renderPos,
                            GeoSceneLayer * layer )
{

//    painter->drawEllipse(coordinate(), screenBounding().width(), screenBounding().height(), true);
    //Would it not be useful to have a draw latlongbox?
//    painter->drawRect(geoBounding());
    qreal north = 0, south = 0, east = 0, west = 0;

    //don't need this if its done by the painter soon
//    geoBounding(viewport->angularResolution());
//    north = m_regions.at(0).boundingRect().top();
//    south = m_regions.at(0).boundingRect().bottom();
//    east = m_regions.at(0).boundingRect().right();
//    west = m_regions.at(0).boundingRect().left();

    //would like a method to draw a QRegion ;)
//    painter->drawRect(GeoDataCoordinates((west + east) /2 , (north + south) / 2, 0 ),
//                      m_regions.at(0).boundingRect().width() * RAD2DEG,
//                      m_regions.at(0).boundingRect().height() * RAD2DEG,true);


    painter->drawPoint( GeoDataCoordinates((west + east) /2 , (north + south) / 2, 0 ) );

    painter->drawPixmap( coordinate(), QPixmap( MarbleDirs::path( "bitmaps/annotation.png" ) )  );

    qreal x, y;
    bool hidden;
    bool visable = viewport->currentProjection()->screenCoordinates( coordinate(), viewport, x, y, hidden );

    if( renderPos == "HOVERS_ABOVE_SURFACE" ) {
        painter->drawPixmap( coordinate(),
                             QPixmap( MarbleDirs::path( "bitmaps/annotation.png" ) )  );
         QList<QRegion> list;
        list.append( QRegion( x -10 , y-10 , 20 , 20 ) );
        setRegions( list );
        painter->drawRect( regions().at(0).boundingRect() );
        return;
    }



    if( visable && !hidden ) {
        bubble->moveTo( QPoint( x, y ) );
        bubble->paint( painter, viewport, renderPos, layer );
    } else {
        bubble->setHidden(true );
    }

    //FIXME This shouldn't really be a part of this method at all as each item should
    //be a part of the scene regardless if it has a parent or not!
    //Parent - Child relationship should only be used in the paint function to decide
    //if the coordinate of the object should be an offset of the parent or an actual
    //coordinate.
    QListIterator<TmpGraphicsItem*> it(children());

    if( it.hasNext() ) {
        TmpGraphicsItem* p = it.next();
        p->paint(painter, viewport, renderPos, layer );
    }




}

QVariant PlacemarkTextAnnotation::itemChange(GeoGraphicsItemChange change, QVariant v )
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

QString PlacemarkTextAnnotation::name() const
{
    return m_textEditor->name();
}

void PlacemarkTextAnnotation::setName( const QString &name )
{
    m_textEditor->setName( name );
}

QString PlacemarkTextAnnotation::description() const
{
    return m_textEditor->description();
}

void PlacemarkTextAnnotation::setDescription( const QString &description )
{
    m_textEditor->setDescription( description );
}

GeoDataPoint PlacemarkTextAnnotation::geometry() const
{
    GeoDataPoint point( coordinate() );
    return point;
}

void PlacemarkTextAnnotation::setGeometry( const GeoDataGeometry &geometry )
{
    //FIXME: undefined reference
//    if( geometry.nodeType() == GeoDataTypes::GeoDataPointType ) {
        static_cast<GeoDataPoint>(geometry);
//    }
}


bool PlacemarkTextAnnotation::mousePressEvent( QMouseEvent* event )
{
    Q_UNUSED(event);
    bubble->setHidden( !bubble->isHidden() );
    return true;
}

}
