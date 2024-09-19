// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>

#ifndef MARBLE_KML_KMLANIMATEDUPDATETAGHANDLER_H
#define MARBLE_KML_KMLANIMATEDUPDATETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{
namespace gx
{
class KmlAnimatedUpdateTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};
}
}
}

#endif
