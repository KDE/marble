//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef OSMBOUNDSGRAPHICSITEM_H
#define OSMBOUNDSGRAPHICSITEM_H

#include "GeoDataLineString.h"

namespace Marble{

class GeoPainter;
class ViewportParams;
class GeoSceneLayer;

class OsmBoundsGraphicsItem : public GeoDataLineString
{
public:
    OsmBoundsGraphicsItem();
    OsmBoundsGraphicsItem( GeoDataGeometry& other );

    virtual void paint( GeoPainter* painter, ViewportParams *viewport,
                        const QString &renderPos, GeoSceneLayer *layer );
};

}

#endif // OSMBOUNDSGRAPHICSITEM_H
