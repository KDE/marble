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

#include "GeoSceneGroup.h"

#include <QtCore/QDebug>

#include "GeoSceneProperty.h"

GeoSceneGroup::GeoSceneGroup( const QString& name )
    : m_name( name )
{
    /* NOOP */
}

GeoSceneGroup::~GeoSceneGroup()
{
    qDeleteAll( m_properties );
}

void GeoSceneGroup::addProperty( GeoSceneProperty* property )
{
    // Remove any property that has the same name
    QVector<GeoSceneProperty*>::iterator it = m_properties.begin();
    while (it != m_properties.end()) {
        GeoSceneProperty* currentProperty = *it;
        if ( currentProperty->name() == property->name() ) {
            delete currentProperty;
            it = m_properties.erase(it);
        }
        else {
            ++it;
        }
     }

    if ( property ) {
        m_properties.append( property );
    }
}

GeoSceneProperty* GeoSceneGroup::property( const QString& name )
{
    GeoSceneProperty* property = 0;

    QVector<GeoSceneProperty*>::const_iterator it = m_properties.begin();
    for (it = m_properties.begin(); it != m_properties.end(); ++it) {
        if ( (*it)->name() == name )
            property = *it;
    }

    if ( property ) {
        Q_ASSERT(property->name() == name);
        return property;
    }

    property = new GeoSceneProperty( name );
    addProperty( property );

    return property;
}

QVector<GeoSceneProperty*> GeoSceneGroup::properties() const
{
    return m_properties;
}

QString GeoSceneGroup::name() const
{
    return m_name;
}
