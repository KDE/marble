//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef MARBLE_GEOLINESTRINGGRAPHICSITEM_H
#define MARBLE_GEOLINESTRINGGRAPHICSITEM_H

#include "GeoDataLineString.h"
#include "GeoGraphicsItem.h"
#include "marble_export.h"

namespace Marble
{
class GeoDataLineStyle;
class MARBLE_EXPORT GeoLineStringGraphicsItem : public GeoGraphicsItem
{
public:
    GeoLineStringGraphicsItem( const GeoDataLineString *lineString );

    void setLineString( const GeoDataLineString* lineString );

    virtual GeoDataCoordinates coordinate() const;
    virtual void coordinate( qreal &longitude, qreal &latitude, qreal &altitude ) const;

    virtual GeoDataLatLonAltBox latLonAltBox() const;

    virtual void paint( GeoPainter* painter, ViewportParams *viewport,
                        const QString &renderPos, GeoSceneLayer *layer );

protected:
    const GeoDataLineString *m_lineString;
};

}

#endif
