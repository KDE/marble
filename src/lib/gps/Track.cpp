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
#include "BoundingBox.h"
#include <QDebug>


Track::Track() : 
    AbstractLayerContainer(),
    AbstractLayerData(0,0),
    GpsElement()
{
}

Track::~Track() 
{
    iterator it;
    for ( it = this->begin(); it < this->constEnd(); it++ ) {
        delete (*it);
    }
    clear();
}

void Track::draw(ClipPainter *painter, const QSize &canvasSize, 
                 ViewParams *viewParams )
{
    const_iterator it;
    for ( it = this->begin(); it < this->constEnd(); it++ ) {
        ( *it ) -> draw( painter, canvasSize, viewParams );
    }
}

void Track::draw(ClipPainter *painter, const QSize &canvasSize, 
                 ViewParams *viewParams, BoundingBox box)
{
#if 0
    qDebug()<<"test er";
    if ( box.isValid() ) {
        if ( box.intersects( *m_boundingBox ) ) {
            qDebug() << "drawing";
            draw( painter, canvasSize, radius, invRotAxis );
        }
        else {
            qDebug() << "not Drawing Track";
            qDebug() << box.string();
            qDebug() << m_boundingBox->string();
            return;
        }
    }
    else {
        qDebug()<<"implementing but not using";
        //bouding box doesn't work so draw anyway
        draw( painter, canvasSize, radius, invRotAxis );
    }
#endif
    const_iterator it;
    for( it = this->begin(); it < this->constEnd(); it++ ) {
        ( *it ) -> draw( painter, canvasSize, viewParams );
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
