// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

#ifndef KMLMEMBERTAGHANDLER_H
#define KMLMEMBERTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlmemberTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
