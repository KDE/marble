//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef KMLEASTTAGHANDLER_H
#define KMLEASTTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmleastTagHandler : public GeoTagHandler
{
 public:
    virtual GeoNode* parse(GeoParser&) const;
};

}
}

#endif // KMLEASTTAGHANDLER_H
