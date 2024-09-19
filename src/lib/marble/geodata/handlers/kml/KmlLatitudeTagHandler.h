// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
//

#ifndef KmlLatitudeTagHandler_h
#define KmlLatitudeTagHandler_h

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmllatitudeTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
