//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "VectorMapBaseLayer.h"

#include "VectorComposer.h"

namespace Marble
{

VectorMapBaseLayer::VectorMapBaseLayer( VectorComposer *vectorComposer )
    : m_vectorComposer( vectorComposer )
{
}

QStringList VectorMapBaseLayer::renderPosition() const
{
    return QStringList() << "SURFACE";
}

bool VectorMapBaseLayer::render( GeoPainter *painter,
                                 ViewportParams *viewport,
                                 const QString &renderPos,
                                 GeoSceneLayer *layer )
{
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    m_vectorComposer->paintBaseVectorMap( painter, viewport );

    return true;
}

qreal VectorMapBaseLayer::zValue() const
{
    return -100.0;
}

}
