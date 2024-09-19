/*
    SPDX-FileCopyrightText: 2010 Thibaut GRIDEL <tgridel@free.fr>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef MARBLE_GPX_TRKPTTAGHANDLER_H
#define MARBLE_GPX_TRKPTTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace gpx
{

class GPXtrkptTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
