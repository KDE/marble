// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef KmlTimeStampTagHandler_h
#define KmlTimeStampTagHandler_h

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlTimeStampTagHandler : public GeoTagHandler
{
public:
    GeoNode* parse(GeoParser&) const override;
};

}
}

#endif // KmlTimeStampTagHandler_h
