//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#ifndef LINESTRINGHANDLER_H
#define LINESTRINGHANDLER_H

#include "PlacemarkFilter.h"

class LineStringProcessor : public PlacemarkFilter
{
public:
    explicit LineStringProcessor(GeoDataDocument* document);

    virtual void process();

};

#endif // LINESTRINGHANDLER_H
