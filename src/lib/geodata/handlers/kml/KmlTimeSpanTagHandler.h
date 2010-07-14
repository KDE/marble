//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
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
    virtual GeoNode* parse(GeoParser&) const;
};

}
}

#endif // KmlTimeSpanTagHandler_h
