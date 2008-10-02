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
#include "BoundingBox.h"

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
    //temparory item to keep track of previous point
    AbstractLayerData *first=0;

    //record the posisions of the points
    QPoint firstPos;
    QPoint secondPos;
    const_iterator it;

    // Initialise first to the beginning of the vector.
    first = *(begin());

    for ( it = constBegin(); it < constEnd(); ++it){
        first->getPixelPos( canvasSize, viewParams,
                            &firstPos );

        (*it)->getPixelPos( canvasSize, viewParams , 
                            &secondPos );

        if ( distance( firstPos, secondPos ) > 25 ) {
            first->draw( painter, firstPos );
            (*it)->draw( painter, secondPos );
            painter->drawLine( firstPos, secondPos );
        }
    }
}

void Route::draw( ClipPainter *painter, 
                  const QSize &canvasSize, ViewParams *viewParams,
                  const BoundingBox &box )
{
    Q_UNUSED( painter );
    Q_UNUSED( canvasSize );
    Q_UNUSED( viewParams );
    Q_UNUSED( box );
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
