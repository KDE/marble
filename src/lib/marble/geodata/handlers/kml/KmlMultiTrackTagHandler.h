// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLE_KML_KMLMULTITRACKTAGHANDLER_H
#define MARBLE_KML_KMLMULTITRACKTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlMultiTrackTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
