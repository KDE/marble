// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Anders Lund <anders@alweb.dk>
//

#ifndef MARBLE_GPX_TYPETAGHANDLER_H
#define MARBLE_GPX_TYPETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace gpx
{

class GPXtypeTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
