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

#include "GeoGraphicsItem.h"
#include "marble_export.h"

namespace Marble
{

class GeoDataPlacemark;
class GeoDataPoint;

class MARBLE_EXPORT GeoPointGraphicsItem : public GeoGraphicsItem
{
public:
    explicit GeoPointGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataPoint *point);

    virtual void paint(GeoPainter* painter, const ViewportParams *viewport, const QString &layer);

    virtual const GeoDataLatLonAltBox& latLonAltBox() const;

protected:
    const GeoDataPoint *m_point;
};

}

#endif
