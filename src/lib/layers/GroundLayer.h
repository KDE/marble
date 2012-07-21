//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010,2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2011      Jens-Michael Hoffmann <jmho@c-xx.com>
// Copyright 2012      Cezar Mocan <mocancezar@gmail.com>
//

#ifndef MARBLE_GROUNDLAYER_H
#define MARBLE_GROUNDLAYER_H

#include "LayerInterface.h"

namespace Marble
{

class GroundLayer : public LayerInterface
{
public:
    virtual QStringList renderPosition() const;

    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
    const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 );

    virtual qreal zValue() const;
};

}

#endif
