// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_KMLMXSIZETAGHANDLER_H
#define MARBLE_KMLMXSIZETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{
namespace mx
{

class KmlsizeTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}
}

#endif
