//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>
//


// Own
#include "Track.h"

// QT
#include "MarbleDebug.h"


using namespace Marble;

Track::Track() : 
    AbstractLayerContainer(),
    AbstractLayerData(0,0),
    GpsElement()
{
}

Track::~Track() 
{
    iterator it;
    for ( it = this->begin(); it < this->end(); ++it ) {
        delete (*it);
    }
    clear();
}

void Track::draw(ClipPainter *painter, const QSize &canvasSize, 
                 ViewParams *viewParams )
{
    const_iterator it;
    for ( it = this->constBegin(); it < this->constEnd(); ++it ) {
        (*it)->draw( painter, canvasSize, viewParams );
    }
}

void Track::draw( ClipPainter*, const QPoint&)
{
    //a track does not have a single point drawing implementation
}

void Track::printToStream( QTextStream &out ) const
{
    out     << "<trk>\n";
    AbstractLayerContainer::printToStream( out );
    out     << "</trk>\n";
}
