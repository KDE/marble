//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#include "Settings.h"
#include "MarbleDebug.h"

#include <QApplication>
#include <QSettings>

Settings::Settings() :
    m_organizationName( QApplication::organizationName() ),
    m_applicationName( QApplication::applicationName() )
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

bool Settings::debugOutputEnabled() const
{
    return Marble::MarbleDebug::isEnabled();
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

void Settings::remove(const QString &group, const QString &value)
{
    QSettings settings(m_organizationName, m_applicationName);
    settings.beginGroup(group);
    settings.remove(value);
    settings.endGroup();
}

void Settings::setDebugOutputEnabled(bool debugOutputEnabled)
{
    if (Marble::MarbleDebug::isEnabled() == debugOutputEnabled) {
        return;
    }

    Marble::MarbleDebug::setEnabled(debugOutputEnabled);
    emit debugOutputEnabledChanged(Marble::MarbleDebug::isEnabled());
}

#include "moc_Settings.cpp"
