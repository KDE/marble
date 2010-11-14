//
// This file is part of the Marble Virtual Globe.
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
#include "GeoPointGraphicsItem.h"
#include "marble_export.h"

#include <QtGui/QPen>

namespace Marble
{

class OsmNodeGraphicsItem : public GeoPointGraphicsItem
{
public:
    OsmNodeGraphicsItem();

    virtual void paint( GeoPainter* painter, ViewportParams *viewport,
                        const QString &renderPos, GeoSceneLayer *layer );

    void setId( int id );
    int id();


private:
    QPen m_pen;
    int m_id;
};

}

#endif // OSMNODEGRAPHICSITEM_H
