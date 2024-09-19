// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Dennis Nienhüser <nienhueser@kde.org>
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
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
