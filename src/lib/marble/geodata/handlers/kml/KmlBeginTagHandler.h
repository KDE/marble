// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef KmlBeginTagHandler_h
#define KmlBeginTagHandler_h

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlbeginTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif // KmlBeginTagHandler_h
