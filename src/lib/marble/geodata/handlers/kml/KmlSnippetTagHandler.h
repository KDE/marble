//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Levente Kurusa <levex@linux.com>
//

#ifndef KMLSNIPPETTAGHANDLER_H
#define KMLSNIPPETTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlSnippetTagHandler : public GeoTagHandler
{
public:
    virtual GeoNode * parse( GeoParser & ) const;
};

}
}

#endif
