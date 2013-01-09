//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "Settings.h"

Settings::Settings() :
    m_organizationName( "KDE" ), m_applicationName( "Marble Virtual Globe" )
{
    // nothing to do
}

QString Settings::organizationName() const
{
    return m_organizationName;
}

void Settings::setOrganizationName( const QString &organization )
{
    m_organizationName = organization;
}

QString Settings::applicationName() const
{
    return m_applicationName;
}

void Settings::setApplicationName( const QString &application )
{
    m_applicationName = application;
}

QVariant Settings::value( const QString &group, const QString &key, const QVariant &value ) const
{
    QSettings settings( m_organizationName, m_applicationName );
    settings.beginGroup( group );
    return settings.value( key, value );
}

void Settings::setValue( const QString &group, const QString &key, const QVariant &value )
{
    QSettings settings( m_organizationName, m_applicationName );
    settings.beginGroup( group );
    settings.setValue( key, value );
    settings.endGroup();
}

#include "Settings.moc"
