// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Niko Sams <niko.sams@gmail.com>
//

#ifndef MARBLE_KML_KMLSCHEMADATATAGHANDLER_H
#define MARBLE_KML_KMLSCHEMADATATAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlSchemaDataTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
