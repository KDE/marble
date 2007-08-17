//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//
#ifndef ROUTE_H
#define ROUTE_H

#include "AbstractLayer/AbstractLayerContainer.h"
#include "GpsElement.h"

/**
 * @brief representation a GPS Route.
 * 
 * A GPS Route is an ordered list of waypoints representing a series
 * of turn points leading to a destination.
 */
class Route : public AbstractLayerContainer, public GpsElement, 
              public AbstractLayerData
{
 public:
    /**
     * @brief empty constructor
     */
    Route();
    
    /**
     * @brief draw each of the Waypoints in the route. 
     * @see AbstractLayerContainer::draw()
     */
    virtual void draw( ClipPainter *painter,
                       const QSize &canvasSize, double radius,
                       Quaternion invRotAxis );
   
    virtual void draw(ClipPainter *painter, 
                      const QSize &canvasSize, double radius,
                      Quaternion invRotAxis, BoundingBox box);
    virtual void draw( ClipPainter*, const QPoint& );
    
    virtual void printToStream( QTextStream & ) const;
    
};

#endif //ROUTE_H
