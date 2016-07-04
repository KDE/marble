//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#ifndef PLACEMARKHANDLER_H
#define PLACEMARKHANDLER_H

#include "BaseFilter.h"

class PlacemarkFilter : public BaseFilter
{
public:
    PlacemarkFilter(GeoDataDocument* document);
};

#endif // PLACEMARKHANDLER_H
