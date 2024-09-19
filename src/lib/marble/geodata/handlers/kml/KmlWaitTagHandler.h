// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mihail Ivchenko <ematirov@gmail.com>
//

#ifndef KMLWAITTAGHANDLER_H
#define KMLWAITTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlWaitTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &parser) const override;
};

} // namespace kml
} // namespace Marble

#endif // KMLWAITTAGHANDLER_H
