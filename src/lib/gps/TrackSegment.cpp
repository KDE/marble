//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//

#include "TrackSegment.h"
#include "ClipPainter.h"
#include "MarbleDirs.h"

#include <QtCore/QPoint>
#include <QtGui/QBrush>
#include <QtGui/QColor>

#include <cmath>
#include "MarbleDebug.h"

using namespace Marble;

TrackSegment::TrackSegment()
    :AbstractLayerContainer(),
    AbstractLayerData( 0, 0 )
{
}

void TrackSegment::draw( ClipPainter *painter, 
                         const QSize &canvasSize,
                         ViewParams *viewParams )
{
    QPoint *previous=0, position;
    bool draw;
    int count = 0;
    
    const_iterator it;
    
    for( it = constBegin(); it < constEnd();
         ++it )
    {
        draw = (*it)->getPixelPos(canvasSize, viewParams,
                 &position);
        if( draw ) {
            //if this is the first TrackPoint in this segment
            if ( previous == 0 ) {
                (*it)->draw( painter, position );
                previous = new QPoint();
                *previous = position;
            } else {
                if ( distance ( position, *previous ) > 25.0 ) {
                    if ( count == 5 ) {
                        drawArrow( painter, (QPointF)*previous,
                                   (QPointF)position );
                        count = 0;
                    }
                    
                    (*it)->draw ( painter, position );
                    *previous= position;
                    
                    count++;
                }
            }
        }
    }

    delete previous;
}

void TrackSegment::draw( ClipPainter*, const QPoint& )
{
    //a track segment does not have a single point drawing
    //implementation
}

void TrackSegment::drawArrow( ClipPainter *painter, 
                               QPointF first, 
                               QPointF second )
{
    //the unit vector in the direction between first and second
    QPointF unitVector =   ( first - second ) 
                         / ( std::sqrt( distance( first, second ) ) );
    //the inverse of the unit vector between first and second
    QPointF unitVector2 = QPointF ( -unitVector.y(), unitVector.x());
    
    
    QPointF mid = ( first + second ) /2;
    
    //(vector/vecLen) is the unit vector
    //QPointF unitVector2 = vector2 / vecLen;
    
    
    QPointF arrowBase = mid + ((unitVector) * 10);
 //   QPointF one (1,1);
    
    QPolygonF arrow;
    arrow   << mid 
            << arrowBase + ((unitVector2) *4)
            << arrowBase - ((unitVector2) *4);
    //save current brush
    QBrush temp = painter->brush();
    painter->setBrush( QBrush( QColor( 232, 87, 82 ) ) );
    
    painter->drawPolygon( arrow );
    
    //reload previous brush
    painter->setBrush( temp );
    
}

void    TrackSegment::printToStream( QTextStream &out ) const
{
    out     << "<trkseg>\n";
    AbstractLayerContainer::printToStream( out );
    out     << "</trkseg>\n";
}
