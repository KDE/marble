// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DGML_TILESIZETAGHANDLER_H
#define MARBLE_DGML_TILESIZETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace dgml
{

class DgmlTileSizeTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
