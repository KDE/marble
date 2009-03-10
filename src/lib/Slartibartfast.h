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
    Returns a pointer to a shared Planet object.
    Use this to create planets when possible.
    @param target the (nonlocalized) name of the planet
    @return a pointer to a shared instance of the planet
    @note if you wish to modify an existing planet, you may get a const Planet
    object here and then use the Planet copy constructor. @see addPlanet()
    */ 
    static const Planet* planetByName(const QString& target);
    /**
    Adds a custom planet to the list.
    @param planet a pointer to the new custom planet. Slartibartfast does not
    take ownership of it.
    @param target the nonlocalized name of the new planet
    @param overwrite if false, Slartibartfast will not overwrite an existing
    planet of the same name
    @return a pointer to the planet that was added. If adding the planet failed,
    it returns a null pointer. Note that it will not always equal @param planet
    since if there is an existing planet and @p overwrite is true, it assigns
    the exisiting planet the value of @p planet, so anyone already using that
    planet gets the new value.
    */
    static const Planet* addPlanet( Planet *planet, const QString& target,
                                    bool overwrite = false );
};

}

#endif
