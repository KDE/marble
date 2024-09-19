// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Anders Lund <anders@alweb.dk>
// SPDX-FileCopyrightText: 2009 Thibaut GRIDEL <tgridel@free.fr>
//

#ifndef MARBLE_GPX_DESCTAGHANDLER_H
#define MARBLE_GPX_DESCTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace gpx
{

class GPXdescTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
