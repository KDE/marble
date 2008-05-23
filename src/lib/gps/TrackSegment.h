//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//

#ifndef TRACKSEG_H
#define TRACKSEG_H

#include "AbstractLayer/AbstractLayerContainer.h"

/**
 * @brief uninterupted sequence of trackPoints
 * 
 * A track is made up of tracksegments that record a sequence of track
 * points. if signal is lost at any time from the gps device then a 
 * new tracksegment should be used when the signal is recovered.
 */
class TrackSegment : public AbstractLayerContainer, 
                     public AbstractLayerData
{
 public:
    TrackSegment();
    virtual void draw(ClipPainter *painter, 
                      const QSize &canvasSize, ViewParams *viewParams,
                      const BoundingBox &box );
    virtual void draw(ClipPainter *painter, 
                      const QSize &canvasSize,
                      ViewParams *viewParams );
    virtual void draw(ClipPainter*, const QPoint&);
    void drawArrow( ClipPainter*,  QPointF,  QPointF );
    
    virtual void printToStream( QTextStream & ) const;

};

#endif

