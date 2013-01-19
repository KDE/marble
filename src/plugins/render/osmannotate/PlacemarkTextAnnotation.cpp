//
// This file is part of the Marble Virtual Globe.
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

PlacemarkTextAnnotation::PlacemarkTextAnnotation( GeoDataFeature *feature )
        : TmpGraphicsItem( feature )
{
    bubble = new GeoWidgetBubble();

    //FIXME decide who actually owns this widget and setup destruction
    //accordingly
    GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( feature );
    m_textEditor = new TextEditor( placemark );
    bubble->setGeoWidget( m_textEditor );
}

QRect PlacemarkTextAnnotation::screenBounding() const
{
    //FIXME the first 2 should actually offset it
    return QRect(-5,-5,20,20);
}

void PlacemarkTextAnnotation::paint( GeoPainter *painter,
                            const ViewportParams *viewport )
{

    const GeoDataPlacemark *placemark = static_cast<const GeoDataPlacemark*>( feature() );
    painter->drawPixmap( placemark->coordinate(), QPixmap( MarbleDirs::path( "bitmaps/annotation.png" ) )  );

    qreal x, y;
    bool hidden;
    bool visible = viewport->currentProjection()->screenCoordinates( placemark->coordinate(), viewport, x, y, hidden );

    QList<QRegion> list;
    list.append( QRegion( x -10 , y-10 , 20 , 20 ) );
    setRegions( list );

    if( visible && !hidden ) {
        bubble->moveTo( QPoint( x, y ) );
        bubble->paint( painter, viewport );
    } else {
        bubble->setHidden(true );
    }
}

bool PlacemarkTextAnnotation::mousePressEvent( QMouseEvent* event )
{
    Q_UNUSED(event);
    bubble->setHidden( !bubble->isHidden() );
    return true;
}

}
