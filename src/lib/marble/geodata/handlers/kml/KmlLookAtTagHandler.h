// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
//
#ifndef KmlLookAtTagHandler_h
#define KmlLookAtTagHandler_h

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlLookAtTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
