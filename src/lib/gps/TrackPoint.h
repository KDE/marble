//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>
//


#ifndef TRACKPOINT_H
#define TRACKPOINT_H


#include "Waypoint.h"

class ClipPainter;
class QPoint;

/**
 * @brief An element of a Track
 */
class TrackPoint : public Waypoint
{
 public:
    /**
     * @brief simple constructor 
     * 
     * this initialises a trackpoint to a position and any other
     * information elements can be entered layer
     */
    TrackPoint( double lat, double lon );
    
    /**
     * @brief draws a 3 pixel circle for a TrackPoint
     */
    virtual void draw ( ClipPainter *, const QPoint & );
};


#endif
