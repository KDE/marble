//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_KML_KMLLATLONQUADTAGHANDLER_H
#define MARBLE_KML_KMLLATLONQUADTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlLatLonQuadTagHandler : public GeoTagHandler
{
 public:
    virtual GeoNode* parse( GeoParser& ) const;
};

}
}

#endif
