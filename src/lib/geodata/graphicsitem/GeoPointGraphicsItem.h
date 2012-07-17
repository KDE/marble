//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef MARBLE_GEOPOINTGRAPHICSITEM_H
#define MARBLE_GEOPOINTGRAPHICSITEM_H

#include "GeoDataPoint.h"
#include "GeoGraphicsItem.h"
#include "GeoDataLatLonAltBox.h"
#include "marble_export.h"

namespace Marble
{

class MARBLE_EXPORT GeoPointGraphicsItem : public GeoGraphicsItem
{
public:
    GeoPointGraphicsItem();

    void setPoint( const GeoDataPoint& point );
    GeoDataPoint point() const;
    
    virtual void paint( GeoPainter* painter, ViewportParams *viewport,
                        const QString &renderPos, GeoSceneLayer *layer );

    virtual GeoDataLatLonAltBox& latLonAltBox() const;

protected:
    GeoDataPoint    m_point;
};

}

#endif
