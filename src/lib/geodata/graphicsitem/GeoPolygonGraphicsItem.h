//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#ifndef MARBLE_GEOPOLYGONGRAPHICSITEM_H
#define MARBLE_GEOPOLYGONGRAPHICSITEM_H

#include "GeoDataPolygon.h"
#include "GeoGraphicsItem.h"
#include "marble_export.h"

namespace Marble
{
class MARBLE_EXPORT GeoPolygonGraphicsItem : public GeoGraphicsItem
{
public:
    GeoPolygonGraphicsItem( const GeoDataPolygon* polygon );
    GeoPolygonGraphicsItem( const GeoDataLinearRing* ring );

    void setPolygon( const GeoDataPolygon* polygon );
    void setLinearRing( const GeoDataLinearRing* ring );

    virtual GeoDataCoordinates coordinate() const;
    virtual void coordinate( qreal &longitude, qreal &latitude, qreal &altitude ) const;

    virtual GeoDataLatLonAltBox latLonAltBox() const;

    virtual void paint( GeoPainter* painter, ViewportParams *viewport,
                        const QString &renderPos, GeoSceneLayer *layer );

protected:
    const GeoDataPolygon *m_polygon;
    const GeoDataLinearRing *m_ring;
};

}

#endif
