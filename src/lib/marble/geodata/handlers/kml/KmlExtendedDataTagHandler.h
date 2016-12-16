//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Harshit Jain <hjain.itbhu@gmail.com>
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
    GeoNode* parse( GeoParser& ) const override;
};

}
}

#endif
