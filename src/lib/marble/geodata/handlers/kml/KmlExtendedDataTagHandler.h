// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef KmlExtendedDataTagHandler_h
#define KmlExtendedDataTagHandler_h

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlExtendedDataTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
