//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//
#ifndef WAYPOINTCONTAINER_H
#define WAYPOINTCONTAINER_H

#include "AbstractLayer/AbstractLayerContainer.h"

class WaypointContainer : AbstractLayerContainer
{
 public:
    WaypointContainer();
    WaypointContainer( QString name ):AbstractLayerContainer( name ){}
 protected:
    virtual void processVisable();
};

#endif //WAYPOINTCONTAINER_H
