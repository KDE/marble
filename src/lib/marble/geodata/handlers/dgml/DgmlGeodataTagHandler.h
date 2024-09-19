/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_DGML_GEODATATAGHANDLER_H
#define MARBLE_DGML_GEODATATAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace dgml
{

class DgmlGeodataTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
