/*
    Copyright (C) 2008 Torsten Rahn <rahn@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "GeoSceneSettings.h"

#include "GeoSceneProperty.h"
#include "GeoSceneGroup.h"

GeoSceneSettings::GeoSceneSettings()
{
    /* NOOP */
}

GeoSceneSettings::~GeoSceneSettings()
{
    qDeleteAll(m_properties);
    qDeleteAll(m_groups);
}

void GeoSceneSettings::addGroup( GeoSceneGroup* group )
{
    if ( group ) {
        m_groups.insert( group->name(), group );
    }
}

GeoSceneGroup* GeoSceneSettings::group( const QString& name )
{
    GeoSceneGroup* group = m_groups.value(name);
    if ( group ) {
        Q_ASSERT(group->name() == name);
        return group;
    }

    group = new GeoSceneGroup( name );
    addGroup( group );

    return group;
}

void GeoSceneSettings::addProperty( GeoSceneProperty* property )
{
    if ( property ) {
        m_properties.insert( property->name(), property );
    }
}

GeoSceneProperty* GeoSceneSettings::property( const QString& name )
{
    GeoSceneProperty* property = m_properties.value(name);
    if ( property ) {
        Q_ASSERT(property->name() == name);
        return property;
    }

    property = new GeoSceneProperty( name );
    addProperty( property );

    return property;
}
