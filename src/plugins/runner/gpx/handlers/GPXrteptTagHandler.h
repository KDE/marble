/*
    SPDX-FileCopyrightText: 2010 Thibaut GRIDEL <tgridel@free.fr>
    SPDX-FileCopyrightText: 2011 Anders Lund <anders@alweb.dk>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef MARBLE_GPX_RTEPTTAGHANDLER_H
#define MARBLE_GPX_RTEPTTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace gpx
{

class GPXrteptTagHandler : public GeoTagHandler
{
public:
    GeoNode* parse(GeoParser&) const override;
};

}
}

#endif
