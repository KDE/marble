//Copyright 2009 Henry de Valence <hdevalence@gmail.com>
//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//

#ifndef SLARTIBARTFAST_H
#define SLARTIBARTFAST_H

class QString;

#include "marble_export.h"

namespace Marble {

class Planet;

/**
@short A class to produce planets on demand
This class is used to get Planet objects from a name.
@note Specializes in custom-made luxury planet building.
*/
class MARBLE_EXPORT Slartibartfast
{
public:
    /**
    Returns a pointer to a (possibly shared) Planet object.
    Use this to create planets when possible.
    @param target the (nonlocalized) name of the planet
    @return a pointer to a shared instance of the planet
    */ //FIXME: should the planet be const?    
    static Planet* planetByName(const QString& target);
};

}

#endif
