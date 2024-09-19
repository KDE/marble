// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_KML_KMLTRACKTAGHANDLER_H
#define MARBLE_KML_KMLTRACKTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlTrackTagHandler : public Marble::GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &parser) const override;
};

}
}

#endif // MARBLE_KML_KMLTRACKTAGHANDLER_H
