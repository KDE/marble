/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#ifndef MARBLE_DGML_VECTORTILETAGHANDLER_H
#define MARBLE_DGML_VECTORTILETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace dgml
{

class DgmlVectortileTagHandler : public GeoTagHandler
{
public:
    virtual GeoNode* parse( GeoParser& ) const;
};

}
}

#endif // MARBLE_DGML_VECTORTILETAGHANDLER_H
