//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#ifndef TINYPLANETPROCESSOR_H
#define TINYPLANETPROCESSOR_H

#include "BaseFilter.h"
#include "OsmPlacemarkData.h"

#include <GeoDataLatLonBox.h>

namespace Marble {

class VectorClipper : public BaseFilter
{
public:
    explicit VectorClipper(GeoDataDocument* document);

    GeoDataDocument* clipTo(const GeoDataLatLonBox &box);
    GeoDataDocument* clipTo(unsigned int zoomLevel, unsigned int tileX, unsigned int tileY);

private:
    void copyTags(const GeoDataPlacemark &source, GeoDataPlacemark &target) const;
    void copyTags(const OsmPlacemarkData &originalPlacemarkData, OsmPlacemarkData& targetOsmData) const;
};

}

#endif // TINYPLANETPROCESSOR_H
