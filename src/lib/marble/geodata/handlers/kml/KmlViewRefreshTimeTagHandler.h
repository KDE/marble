// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#ifndef KMLVIEWREFRESHTIMETAGHANDLER_H
#define KMLVIEWREFRESHTIMETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlviewRefreshTimeTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
