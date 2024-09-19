// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Niko Sams <niko.sams@gmail.com>
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "RoutingProfile.h"

namespace Marble
{

RoutingProfile::RoutingProfile(const QString &name)
    : m_name(name)
    , m_transportType(Motorcar)
{
    // nothing to do
}

QString RoutingProfile::name() const
{
    return m_name;
}

void RoutingProfile::setName(const QString &name)
{
    m_name = name;
}

const QHash<QString, QHash<QString, QVariant>> &RoutingProfile::pluginSettings() const
{
    return m_pluginSettings;
}

QHash<QString, QHash<QString, QVariant>> &RoutingProfile::pluginSettings()
{
    return m_pluginSettings;
}

void RoutingProfile::setTransportType(RoutingProfile::TransportType transportType)
{
    m_transportType = transportType;
}

RoutingProfile::TransportType RoutingProfile::transportType() const
{
    return m_transportType;
}

bool RoutingProfile::operator==(const RoutingProfile &other) const
{
    return m_name == other.name() && m_pluginSettings == other.pluginSettings();
}

}
