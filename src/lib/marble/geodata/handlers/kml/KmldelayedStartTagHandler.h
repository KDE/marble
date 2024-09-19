// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef KMLDELAYEDSTARTTAGHANDLER_H
#define KMLDELAYEDSTARTTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmldelayedStartTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &parser) const override;
};

} // namespace kml
} // namespace Marble

#endif // KMLDELAYEDSTARTTAGHANDLER_H
