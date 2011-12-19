//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Niko Sams <niko.sams@gmail.com>
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutingProfile.h"

namespace Marble
{

RoutingProfile::RoutingProfile( const QString &name ) :
    m_name( name ), m_transportType( Motorcar )
{
    // nothing to do
}

QString RoutingProfile::name() const
{
    return m_name;
}

void RoutingProfile::setName( const QString &name )
{
    m_name = name;
}

const QHash<QString, QHash<QString, QVariant> >& RoutingProfile::pluginSettings() const
{
    return m_pluginSettings;
}

QHash<QString, QHash<QString, QVariant> >& RoutingProfile::pluginSettings()
{
    return m_pluginSettings;
}

void RoutingProfile::setTransportType( RoutingProfile::TransportType transportType )
{
    m_transportType = transportType;
}

RoutingProfile::TransportType RoutingProfile::transportType() const
{
    return m_transportType;
}

bool RoutingProfile::operator==( const RoutingProfile &other ) const
{
    return m_name == other.name() && m_pluginSettings == other.pluginSettings();
}

}
