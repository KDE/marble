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

#include <QColor>

namespace Marble
{

class GroundLayer : public LayerInterface
{
 public:

    GroundLayer();

    ~GroundLayer();

    virtual QStringList renderPosition() const;

    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
    const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 );

    virtual qreal zValue() const;

    void setColor( const QColor &color );

    QColor color() const;

 private:
    bool m_enabled;  // If the current map theme has texture layers then m_enabled will be set to false

    QColor m_color;  // Gets the color specified via DGML's <map bgcolor="">
    
};

}

#endif
