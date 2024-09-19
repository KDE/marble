// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
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
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
