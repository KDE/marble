//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//
#ifndef MARBLE_ROUTE_H
#define MARBLE_ROUTE_H

#include "AbstractLayerContainer.h"
#include "AbstractLayerData.h"
#include "GpsElement.h"

namespace Marble
{

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
                       const QSize &canvasSize, 
                       ViewParams *viewParams );

    virtual void draw( ClipPainter*, const QPoint& );

    virtual void printToStream( QTextStream & ) const;

};

}

#endif
