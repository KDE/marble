//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009    Torsten Rahn <tackat@kde.org>
//

//
// Description: AbstractTile contains the base class for a single quadtile 
//


#ifndef MARBLE_GLOBAL_P_H
#define MARBLE_GLOBAL_P_H

#include "MarbleLocale.h"

#include "global.h"

namespace Marble
{


class MarbleGlobalPrivate
{
 public:
    MarbleGlobalPrivate();
    
    virtual ~MarbleGlobalPrivate();

    MarbleLocale m_locale;
    
    MarbleGlobal::Profiles m_profiles;
};

}

#endif // MARBLE_GLOBAL_P_H
