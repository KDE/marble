// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef KmlEndTagHandler_h
#define KmlEndTagHandler_h

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlendTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif // KmlEndTagHandler_h
