//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_KML_KMLMINALTITUDETAGHANDLER_H
#define MARBLE_KML_KMLMINALTITUDETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlminAltitudeTagHandler : public GeoTagHandler
{
 public:
    GeoNode* parse(GeoParser&) const override;
};

}
}

#endif
