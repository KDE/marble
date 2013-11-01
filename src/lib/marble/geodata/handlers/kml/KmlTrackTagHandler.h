//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_KML_KMLTRACKTAGHANDLER_H
#define MARBLE_KML_KMLTRACKTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlTrackTagHandler : public Marble::GeoTagHandler
{

public:
    virtual GeoNode *parse( GeoParser &parser ) const;
};

}
}

#endif // MARBLE_KML_KMLTRACKTAGHANDLER_H
