//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016 Sanjiban Bairagya <sanjibanb@kde.org>
//

#ifndef MARBLE_DGMLRENDERORDERTAGHANDLER_H
#define MARBLE_DGMLRENDERORDERTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace dgml
{

class DgmlRenderOrderTagHandler : public GeoTagHandler
{
public:
    virtual GeoNode* parse(GeoParser&) const;
};

}
}

#endif
