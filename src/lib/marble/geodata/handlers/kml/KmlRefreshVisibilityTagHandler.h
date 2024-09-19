// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//
#ifndef KMLREFRESHVISIBILITYTAGHANDLER_h
#define KMLREFRESHVISIBILITYTAGHANDLER_h

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlrefreshVisibilityTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
