//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef KmlWhenTagHandler_h
#define KmlWhenTagHandler_h

#include <QtCore/QString>
#include "GeoTagHandler.h"
#include <GeoDataTimeStamp.h>

namespace Marble
{
namespace kml
{

class KmlwhenTagHandler : public GeoTagHandler
{
public:
    virtual GeoNode* parse(GeoParser&) const;

private:
    Marble::GeoDataTimeStamp::TimeResolution modify( QString& whenString ) const;
};

}
}

#endif // KmlWhenTagHandler_h
