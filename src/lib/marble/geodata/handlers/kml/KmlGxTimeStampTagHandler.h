// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#ifndef KMLGXTIMESTAMPTAGHANDLER_H
#define KMLGXTIMESTAMPTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{
namespace gx
{

class KmlTimeStampTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}
}

#endif // KMLGXTIMESTAMPTAGHANDLER_H
