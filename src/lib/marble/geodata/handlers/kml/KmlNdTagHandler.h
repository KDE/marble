//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015    Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

#ifndef KMLNDTAGHANDLER_H
#define KMLNDTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{

namespace kml
{

class KmlndTagHandler : public GeoTagHandler
{
public:
    GeoNode* parse( GeoParser& ) const override;
};

}
}

#endif

