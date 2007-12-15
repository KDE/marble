//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>
//

#ifndef ROUTECONTAINER_H
#define ROUTECONTAINER_H


#include "AbstractLayer/AbstractLayerContainer.h"


/**
 * @brief holds a collection of Routes
 */
class RouteContainer : public virtual AbstractLayerContainer
{
 public:
     /*
     virtual void draw ( ClipPainter *painter, const QPoint &point);
     */
    /*
     virtual void draw( ClipPainter *painter, 
                        const QSize &canvasSize, double radius,
                        Quaternion invRotAxis );*/
    
     virtual void draw( ClipPainter *painter, 
                        const QSize &canvasSize,
                        ViewParams *viewParams,
                        BoundingBox box );
};

#endif
