// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_KML_KMLLATLONBOXTAGHANDLER_H
#define MARBLE_KML_KMLLATLONBOXTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlLatLonBoxTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
