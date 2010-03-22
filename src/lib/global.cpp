//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Torsten Rahn <tackat@kde.org>
//

#include "global.h"
#include "global_p.h"


namespace Marble
{

MarbleGlobalPrivate::MarbleGlobalPrivate()
    : m_profile( MarbleGlobal::Default ) 
{
}

MarbleGlobalPrivate::~MarbleGlobalPrivate()
{
}

MarbleGlobal::MarbleGlobal()
    : d ( new MarbleGlobalPrivate )
{
}

MarbleGlobal::~MarbleGlobal()
{
    delete d;
}

MarbleGlobal * MarbleGlobal::getInstance()
{
    static MarbleGlobal instance; 
    return &instance; 
}

MarbleLocale * MarbleGlobal::locale() const
{
    return &d->m_locale; 
}

MarbleGlobal::Profile MarbleGlobal::profile() const {
    return d->m_profile;
}

void MarbleGlobal::setProfile( MarbleGlobal::Profile profile ) {
    d->m_profile = profile;
}

}
