// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef KMLSCREENXYTAGHANDLER_H
#define KMLSCREENXYTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlscreenXYTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
