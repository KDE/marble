// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef KmlTimeSpanTagHandler_h
#define KmlTimeSpanTagHandler_h

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlTimeSpanTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif // KmlTimeSpanTagHandler_h
