//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutingWaypoint.h"

#include <QtCore/QDebug>

namespace Marble
{

RoutingWaypoint::RoutingWaypoint() : m_secondsRemaining( 0 )
{
    // nothing to do
}

RoutingWaypoint::RoutingWaypoint( const RoutingPoint &point, JunctionType junctionType,
                                  const QString &junctionTypeRaw, const QString &roadType,
                                  int secondsRemaining, const QString &roadName )
        : m_point( point ), m_junctionType( junctionType ),
        m_junctionTypeRaw(junctionTypeRaw), m_roadType( roadType ),
        m_secondsRemaining( secondsRemaining ), m_roadName( roadName.trimmed() )
{
    // nothing to do
}

RoutingPoint RoutingWaypoint::point() const
{
    return m_point;
}

RoutingWaypoint::JunctionType RoutingWaypoint::junctionType() const
{
    return m_junctionType;
}

QString RoutingWaypoint::junctionTypeRaw() const
{
    return m_junctionTypeRaw;
}

QString RoutingWaypoint::roadType() const
{
    return m_roadType;
}

int RoutingWaypoint::secondsRemaining() const
{
    return m_secondsRemaining;
}

QString RoutingWaypoint::roadName() const
{
    return m_roadName;
}

} // namespace Marble
