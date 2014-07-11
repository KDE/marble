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

// Self
#include "PlacemarkTextAnnotation.h"

// Marble
#include "AbstractProjection.h"
#include "GeoDataPlacemark.h"
#include "GeoPainter.h"
#include "GeoWidgetBubble.h"
#include "ViewportParams.h"
#include "MarbleDirs.h"
#include "SceneGraphicsTypes.h"


namespace Marble
{

PlacemarkTextAnnotation::PlacemarkTextAnnotation( GeoDataPlacemark *placemark ) :
    SceneGraphicsItem( placemark ),
    bubble( new GeoWidgetBubble( placemark ) )
{
    // nothing to do
}

PlacemarkTextAnnotation::~PlacemarkTextAnnotation()
{
    delete bubble;
}

void PlacemarkTextAnnotation::paint( GeoPainter *painter, const ViewportParams *viewport )
{
    m_regionList.clear();
    painter->drawPixmap( placemark()->coordinate(), QPixmap( MarbleDirs::path( "bitmaps/annotation.png" ) ) );

    qreal x, y;
    bool hidden;
    bool visible = viewport->currentProjection()->screenCoordinates( placemark()->coordinate(), viewport, x, y, hidden );

    m_regionList.append( QRegion( x -10 , y -10 , 20 , 20 ) );

    if ( visible && !hidden ) {
        bubble->moveTo( QPoint( x, y ) );
        bubble->paint( painter );
    } else {
        bubble->setHidden(true );
    }
}


bool PlacemarkTextAnnotation::containsPoint( const QPoint &eventPos ) const
{
    foreach ( const QRegion &region, m_regionList ) {
        if ( region.contains( eventPos ) ) {
            return true;
        }
    }

    return false;
}

void PlacemarkTextAnnotation::dealWithItemChange( const SceneGraphicsItem *other )
{
    Q_UNUSED( other );
}

const char *PlacemarkTextAnnotation::graphicType() const
{
    return SceneGraphicsTypes::SceneGraphicPlacemark;
}

bool PlacemarkTextAnnotation::mousePressEvent( QMouseEvent* event )
{
    Q_UNUSED( event );
    bubble->setHidden( !bubble->isHidden() );
    return true;
}

bool PlacemarkTextAnnotation::mouseMoveEvent( QMouseEvent *event )
{
    Q_UNUSED( event );
    return true;
}

bool PlacemarkTextAnnotation::mouseReleaseEvent( QMouseEvent *event )
{
    Q_UNUSED( event );
    return true;
}

void PlacemarkTextAnnotation::dealWithStateChange( SceneGraphicsItem::ActionState previousState )
{
    Q_UNUSED( previousState );
}

}
