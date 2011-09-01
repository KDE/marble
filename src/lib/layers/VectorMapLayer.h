//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_VECTORMAPLAYER_H
#define MARBLE_VECTORMAPLAYER_H

#include "LayerInterface.h"

namespace Marble
{

class VectorComposer;

class VectorMapLayer : public LayerInterface
{
public:
    VectorMapLayer( VectorComposer *vectorComposer );

    /**
     * @reimp
     */
    virtual QStringList renderPosition() const;

    /**
     * @reimp
     */
    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
       const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 );

    /**
     * @reimp
     */
    virtual qreal zValue() const;

private:
    VectorComposer *const m_vectorComposer;
};

}

#endif
