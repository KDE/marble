//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#ifndef MARBLE_OSMTAGTAGHANDLER_H
#define MARBLE_OSMTAGTAGHANDLER_H

#include "GeoTagHandler.h"
namespace Marble
{
namespace osm
{

class OsmTagTagHandler : public GeoTagHandler
{
public:
    virtual GeoNode* parse(GeoParser&) const;
};

}

}

#endif // MARBLE_OSMTAGTAGHANDLER_H
