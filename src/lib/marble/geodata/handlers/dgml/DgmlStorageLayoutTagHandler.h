/*
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_DGML_STORAGELAYOUTTAGHANDLER_H
#define MARBLE_DGML_STORAGELAYOUTTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace dgml
{

class DgmlStorageLayoutTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
