//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//


#include "PointScreenGraphicsItem.h"

#include "GeoPainter.h"

namespace Marble
{

PointScreenGraphicsItem::PointScreenGraphicsItem()
        :ScreenGraphicsItem()
{
}

void PointScreenGraphicsItem:: paint( GeoPainter *painter,
                                      ViewportParams *viewport,
                                      const QString& renderPos,
                                      GeoSceneLayer * layer )
{
    Q_UNUSED( viewport )
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )
    painter->save();

    painter->setPen( Qt::black );

    QRectF point( position() - QPointF( 5, 5 ),
                  position() + QPointF( 5, 5 ) );

    painter->drawRect( point );

    painter->restore();
}

}
