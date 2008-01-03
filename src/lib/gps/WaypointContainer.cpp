//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//


#include "WaypointContainer.h"
#include "BoundingBox.h"


WaypointContainer::WaypointContainer()
{
}

void WaypointContainer::processVisable()
{
}

void WaypointContainer::draw( ClipPainter *painter, 
                           const QSize &canvasSize, ViewParams *viewParams,
                           BoundingBox box )
{
    Q_UNUSED( box );

    // FIXME: Use the bounding box.
    draw( painter, canvasSize, viewParams );
}

void WaypointContainer::draw( ClipPainter *painter, 
                              const QSize &canvasSize,
                              ViewParams *viewParams )
{
    const_iterator it;
    bool draw;
    QPoint position;

    painter->setPen( QPen( Qt::black ) );
    painter->setBrush( QBrush( Qt::white ) );
    
    for ( it = this->begin(); it < this->constEnd(); ++it ) {
        draw = (*it)->getPixelPos( canvasSize, viewParams,
                                   &position );

        if ( draw ) {
            ( *it )->draw( painter, position );
        }
    }
}


