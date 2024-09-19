// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
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
    GeoNode *parse(GeoParser &parser) const override;
};

}
}

#endif
