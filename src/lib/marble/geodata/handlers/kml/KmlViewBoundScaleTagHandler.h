// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#ifndef KMLVIEWBOUNDSCALETAGHANDLER_H
#define KMLVIEWBOUNDSCALETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlviewBoundScaleTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif // KMLVIEWBOUNDSCALETAGHANDLER_H
