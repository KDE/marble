// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#ifndef KMLSCALETAGHANDLER_H
#define KMLSCALETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlScaleTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}
#endif // KMLSCALETAGHANDLER_H
