//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//
#ifndef KmlDurationTagHandler_h
#define KmlDurationTagHandler_h

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmldurationTagHandler : public GeoTagHandler
{
public:
    virtual GeoNode * parse( GeoParser & ) const;
};

}
}

#endif
