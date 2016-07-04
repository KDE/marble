//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#ifndef COASTLINEFILTER_H
#define COASTLINEFILTER_H

#include "PlacemarkFilter.h"

class ShpCoastlineProcessor : public PlacemarkFilter
{
public:
    ShpCoastlineProcessor(GeoDataDocument* document);

    virtual void process();

    GeoDataDocument* cutToTiles(unsigned int zoomLevel, unsigned int tileX, unsigned int tileY);
};

#endif // COASTLINEFILTER_H
