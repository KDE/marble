/*
    SPDX-FileCopyrightText: 2010 Thibaut GRIDEL <tgridel@free.fr>
    SPDX-FileCopyrightText: 2011 Anders Lund <anders@alweb.dk>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef MARBLE_GPX_RTETAGHANDLER_H
#define MARBLE_GPX_RTETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace gpx
{

class GPXrteTagHandler : public GeoTagHandler
{
public:
    GeoNode* parse(GeoParser&) const override;
};

}
}

#endif
