// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008-2009 Torsten Rahn <tackat@kde.org>
//

//
// Description: AbstractTile contains the base class for a single quadtile
//

#ifndef MARBLE_GLOBAL_P_H
#define MARBLE_GLOBAL_P_H

#include "MarbleLocale.h"

#include "MarbleGlobal.h"

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
