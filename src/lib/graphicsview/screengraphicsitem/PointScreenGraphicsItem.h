//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef MARBLE_POINTSCREENGRAPHICSITEM_H
#define MARBLE_POINTSCREENGRAPHICSITEM_H

#include "ScreenGraphicsItem.h"
#include "marble_export.h"

namespace Marble
{

class MARBLE_EXPORT PointScreenGraphicsItem : public ScreenGraphicsItem
{
public:
    PointScreenGraphicsItem();
    
    virtual void paint( GeoPainter *painter, ViewportParams *viewport,
                        const QString& renderPos, GeoSceneLayer * layer = 0 );
};

}

#endif
