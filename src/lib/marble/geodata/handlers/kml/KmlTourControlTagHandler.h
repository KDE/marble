// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef KMLTOURCONTROLTAGHANDLER_H
#define KMLTOURCONTROLTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlTourControlTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &parser) const override;
};

} // namespace kml
} // namespace Marble

#endif // KMLTOURCONTROLTAGHANDLER_H
