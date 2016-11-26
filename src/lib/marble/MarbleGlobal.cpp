//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Torsten Rahn <tackat@kde.org>
//

#include "MarbleGlobal.h"
#include "MarbleGlobal_p.h"


namespace Marble
{

MarbleGlobalPrivate::MarbleGlobalPrivate()
    : m_profiles(MarbleGlobal::Default)
{
#ifdef Q_OS_ANDROID
    m_profiles |= MarbleGlobal::SmallScreen;
    m_profiles |= MarbleGlobal::HighResolution;
#endif

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

MarbleGlobal::Profiles MarbleGlobal::profiles() const {
    return d->m_profiles;
}

void MarbleGlobal::setProfiles( MarbleGlobal::Profiles profiles ) {
    d->m_profiles = profiles;
}

MarbleGlobal::Profiles MarbleGlobal::detectProfiles()
{
    return getInstance()->profiles();
}

}
