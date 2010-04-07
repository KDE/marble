//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>
//
#ifndef MARBLE_TRACKCONTAINER_H
#define MARBLE_TRACKCONTAINER_H


#include "AbstractLayerContainer.h"

namespace Marble
{

/**
 * @brief holds a collection of tracks
 */
class TrackContainer : public AbstractLayerContainer
{
 public:
    /**
     * @brief empty constructor
     */
 //   TrackContainer();
    
    
  //virtual void draw ( ClipPainter *painter, const QPoint &point );*/
    
    
    virtual void draw( ClipPainter *painter, 
                       const QSize &canvasSize,
                       ViewParams *viewParams );
    using AbstractLayerContainer::draw;
    
};

}

#endif
