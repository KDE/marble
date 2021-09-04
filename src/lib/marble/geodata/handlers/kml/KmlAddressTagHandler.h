/*
    SPDX-FileCopyrightText: 2017 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_KML_KMLADDRESSTAGHANDLER_H
#define MARBLE_KML_KMLADDRESSTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmladdressTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
