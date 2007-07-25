//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>
//


#include "Track.h"
#include <QDebug>


Track::Track() : AbstractLayerData(0,0), AbstractLayerContainer(),
                 GpsElement()
{
}

void Track::draw(ClipPainter *painter, const QSize &canvasSize, 
                 double radius, Quaternion invRotAxis)
{
    const_iterator it;
    for( it = this->begin(); it < this->constEnd();
         it++ )
    {
        ( *it ) -> draw( painter, canvasSize, radius, invRotAxis );
    }
}

void Track::draw( ClipPainter*, const QPoint&)
{
    //a track does not have a single point drawing implementation
}
