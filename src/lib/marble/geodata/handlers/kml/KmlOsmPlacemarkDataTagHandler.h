//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015    Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

#ifndef KMLOSMPLACEMARKDATATAGHANDLER_H
#define KMLOSMPLACEMARKDATATAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
class OsmPlacemarkData;

namespace kml
{

class KmlOsmPlacemarkDataTagHandler : public GeoTagHandler
{
public:
    virtual GeoNode* parse( GeoParser& parser ) const;
};

}
}

#endif

