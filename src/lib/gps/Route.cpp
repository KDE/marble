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

Route::Route() : AbstractLayerData( 0 , 0 ), AbstractLayerContainer(),
                 GpsElement()
{
}

void Route::draw( ClipPainter *painter, const QSize &canvasSize,
                  double radius, Quaternion invRotAxis )
{
    //temparory item to keep track of previous point
    AbstractLayerData *first=0;
    
    //record the posisions of the points
    QPoint firstPos;
    QPoint secondPos;
    
    const_iterator it;
    
    //initialise first to the begining of the vector
    first = *(begin());
    
    for ( it = constBegin(); it < constEnd(); it++){
        first->getPixelPos( canvasSize, invRotAxis, (int)radius, 
                                &firstPos );
        
        (*it)->getPixelPos( canvasSize, invRotAxis, (int)radius, 
                                 &secondPos );
        
        if ( distance( firstPos, secondPos ) > 25 ) {
            first->draw( painter, firstPos );
            (*it)->draw( painter, secondPos );
            
            painter->drawLine( firstPos, secondPos );
        }
    }
}
