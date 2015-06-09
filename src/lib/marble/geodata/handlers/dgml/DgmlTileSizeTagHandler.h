// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DGML_TILESIZETAGHANDLER_H
#define MARBLE_DGML_TILESIZETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace dgml
{

class DgmlTileSizeTagHandler : public GeoTagHandler
{
public:
    virtual GeoNode* parse( GeoParser& ) const;
};

}
}

#endif
