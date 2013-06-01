//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
// Copyright 2013      Thibaut Gridel <tgridel@free.fr>
//

#include "PlacemarkTextAnnotation.h"


#include "AbstractProjection.h"
#include "GeoDataPlacemark.h"
#include "GeoDocument.h"
#include "GeoPainter.h"
#include "GeoWidgetBubble.h"
#include "ViewportParams.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"

#include <QtGui/QPixmap>

namespace Marble
{

PlacemarkTextAnnotation::PlacemarkTextAnnotation( GeoDataPlacemark *placemark )
        : SceneGraphicsItem( placemark ),
          bubble( new GeoWidgetBubble( placemark ) )
{
}

PlacemarkTextAnnotation::~PlacemarkTextAnnotation()
{
    delete bubble;
}

void PlacemarkTextAnnotation::paint( GeoPainter *painter,
                            const ViewportParams *viewport )
{

    painter->drawPixmap( placemark()->coordinate(), QPixmap( MarbleDirs::path( "bitmaps/annotation.png" ) )  );

    qreal x, y;
    bool hidden;
    bool visible = viewport->currentProjection()->screenCoordinates( placemark()->coordinate(), viewport, x, y, hidden );

    QList<QRegion> list;
    list.append( QRegion( x -10 , y-10 , 20 , 20 ) );
    setRegions( list );

    if( visible && !hidden ) {
        bubble->moveTo( QPoint( x, y ) );
        bubble->paint( painter );
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
