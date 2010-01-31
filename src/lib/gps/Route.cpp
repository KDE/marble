//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//

#include "Route.h"
#include "ClipPainter.h"

#include <QtCore/QPoint>

using namespace Marble;

Route::Route() :  AbstractLayerContainer(),
                  GpsElement(),
                  AbstractLayerData(0,0)
{
}

void Route::draw( ClipPainter *painter, const QSize &canvasSize,
                  ViewParams *viewParams )
{
    //temporary item to keep track of previous point
    AbstractLayerData *first=0;

    //record the positions of the points
    QPoint firstPos;
    QPoint secondPos;
    iterator it;

    // Initialise first to the beginning of the vector.
    first = *(begin());

    for ( it = begin(); it < end(); ++it){
        bool valid = first->getPixelPos( canvasSize, viewParams,
                            &firstPos );

        valid = valid && (*it)->getPixelPos( canvasSize, viewParams ,
                            &secondPos );

        if ( valid && distance( firstPos, secondPos ) > 25 ) {
            first->draw( painter, firstPos );
            (*it)->draw( painter, secondPos );
            painter->drawLine( firstPos, secondPos );
        }

        first = *it;
    }
}

void Route::draw( ClipPainter*, const QPoint& )
{
}

void Route::printToStream( QTextStream &out ) const
{
    out     << "<rte>";
    AbstractLayerContainer::printToStream( out );
    out     << "</rte>";
}
