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

#include "PlacemarkFilter.h"
#include "OsmPlacemarkData.h"

class TinyPlanetProcessor : public PlacemarkFilter
{
public:
    explicit TinyPlanetProcessor(GeoDataDocument* document);

    virtual void process();

    GeoDataDocument* cutToTiles(unsigned int zoomLevel, unsigned int tileX, unsigned int tileY);

private:
    void copyTags(const GeoDataPlacemark &source, GeoDataPlacemark &target) const;
    void copyTags(const OsmPlacemarkData &originalPlacemarkData, OsmPlacemarkData& targetOsmData) const;
};

#endif // TINYPLANETPROCESSOR_H
