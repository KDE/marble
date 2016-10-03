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

#include "AbstractGeoPolygonGraphicsItem.h"

namespace Marble
{

class GeoDataLinearRing;
class GeoDataPolygon;

class MARBLE_EXPORT GeoPolygonGraphicsItem : public AbstractGeoPolygonGraphicsItem
{
public:
    static AbstractGeoPolygonGraphicsItem *createGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataPolygon *polygon);
    static AbstractGeoPolygonGraphicsItem *createGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataLinearRing *ring);

    explicit GeoPolygonGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataPolygon *polygon);
    explicit GeoPolygonGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataLinearRing *ring);
};

}

#endif
