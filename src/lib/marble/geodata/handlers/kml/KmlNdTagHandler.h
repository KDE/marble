// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

#ifndef KMLNDTAGHANDLER_H
#define KMLNDTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{

namespace kml
{

class KmlndTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
