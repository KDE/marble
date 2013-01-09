/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#ifndef MARBLE_OSMMEMBERTAGHANDLER_H
#define MARBLE_OSMMEMBERTAGHANDLER_H

#include "GeoTagHandler.h"
#include "marble_export.h"

namespace Marble
{
namespace osm
{

class OsmMemberTagHandler : public GeoTagHandler
{
public:
    virtual GeoNode* parse( GeoParser& ) const;
};

}

}

#endif // MARBLE_OSMMEMBERTAGHANDLER_H

