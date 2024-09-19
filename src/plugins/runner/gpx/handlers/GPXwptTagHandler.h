/*
    SPDX-FileCopyrightText: 2009 Thibaut GRIDEL <tgridel@free.fr>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef MARBLE_GPX_WPTTAGHANDLER_H
#define MARBLE_GPX_WPTTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace gpx
{

class GPXwptTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
