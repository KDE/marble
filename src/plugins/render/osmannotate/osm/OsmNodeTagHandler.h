//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef OSMNODETAGHANDLER_H
#define OSMNODETAGHANDLER_H

#include "GeoTagHandler.h"
#include "marble_export.h"

namespace Marble
{

class OsmNodeGraphicsItem;

namespace osm
{

class OsmNodeTagHandler : public GeoTagHandler
{
public:
    virtual GeoNode* parse(GeoParser&) const;

    static QMap<int, OsmNodeGraphicsItem*> nodeRef;
};

}

}

#endif // OSMNODETAGHANDLER_H
