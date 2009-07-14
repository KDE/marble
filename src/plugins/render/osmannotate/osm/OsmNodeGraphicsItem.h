//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef OSMNODEGRAPHICSITEM_H
#define OSMNODEGRAPHICSITEM_H

#include "GeoDataPoint.h"
#include "GeoGraphicsItem.h"

#include <QtGui/QPen>

namespace Marble
{

class OsmNodeGraphicsItem : public GeoGraphicsItem
{
public:
    OsmNodeGraphicsItem();

    virtual void paint( GeoPainter* painter, ViewportParams *viewport,
                        const QString &renderPos, GeoSceneLayer *layer );

    void setPoint( const GeoDataCoordinates& point );

private:
    QPen m_pen;
    //FIXME this should be a GeoDataPoint. In fact there should be a
    // GeoPointGraphicsItem that this class should subclass
    GeoDataCoordinates m_point;
};

}

#endif // OSMNODEGRAPHICSITEM_H
