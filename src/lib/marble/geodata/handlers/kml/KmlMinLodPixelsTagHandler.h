//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_KML_KMLMINLODPIXELSTAGHANDLER_H
#define MARBLE_KML_KMLMINLODPIXELSTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlminLodPixelsTagHandler : public GeoTagHandler
{
 public:
    GeoNode* parse(GeoParser&) const override;
};

}
}

#endif
