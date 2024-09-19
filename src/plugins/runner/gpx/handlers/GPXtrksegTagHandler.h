/*
    SPDX-FileCopyrightText: 2010 Thibaut GRIDEL <tgridel@free.fr>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef MARBLE_GPX_TRKSEGTAGHANDLER_H
#define MARBLE_GPX_TRKSEGTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace gpx
{

class GPXtrksegTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
