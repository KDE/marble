// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#ifndef MARBLE_KMLSIMPLEDATATAGHANDLER_H
#define MARBLE_KMLSIMPLEDATATAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlSimpleDataTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}

}

#endif
