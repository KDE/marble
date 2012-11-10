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

#include "MarbleDebug.h"

#include "GeoSceneProperty.h"

namespace Marble
{

GeoSceneGroup::GeoSceneGroup( const QString& name )
    : m_name( name )
{
}

GeoSceneGroup::~GeoSceneGroup()
{
    qDeleteAll( m_properties );
}

bool GeoSceneGroup::propertyAvailable( const QString& name, bool& available ) const
{
    QVector<GeoSceneProperty*>::const_iterator it = m_properties.constBegin();
    QVector<GeoSceneProperty*>::const_iterator end = m_properties.constEnd();
    for (; it != end; ++it) {
        if ( (*it)->name() == name ) {
            available = (*it)->available();
            return true;
        }
    }

    available = false;

    return false;
}

bool GeoSceneGroup::setPropertyValue( const QString& name, bool value )
{
    QVector<GeoSceneProperty*>::const_iterator it = m_properties.constBegin();
    QVector<GeoSceneProperty*>::const_iterator end = m_properties.constEnd();
    for (; it != end; ++it) {
        if ( (*it)->name() == name ) {
            (*it)->setValue( value );
            emit valueChanged( name, value );
            return true;
        }
    }

    return false;
}

bool GeoSceneGroup::propertyValue( const QString& name, bool& value ) const
{
    QVector<GeoSceneProperty*>::const_iterator it = m_properties.constBegin();
    QVector<GeoSceneProperty*>::const_iterator end = m_properties.constEnd();
    for (; it != end; ++it) {
        if ( (*it)->name() == name ) {
            value = (*it)->value();
            return true;
        }
    }

    value = false;

    return false;
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
            break;
        }
        else {
            ++it;
        }
    }

    if ( property ) {
        m_properties.append( property );

        // Establish connection to the outside, e.g. the LegendBrowser
        connect ( property, SIGNAL( valueChanged( QString, bool ) ), 
                            SIGNAL( valueChanged( QString, bool ) ) );
        emit valueChanged( property->name(), property->value() );
    }
}

const GeoSceneProperty* GeoSceneGroup::property( const QString& name ) const
{
    GeoSceneProperty* property = 0;

    QVector<GeoSceneProperty*>::const_iterator it = m_properties.constBegin();
    QVector<GeoSceneProperty*>::const_iterator end = m_properties.constEnd();
    for (; it != end; ++it) {
        if ( (*it)->name() == name ) {
            property = *it;
            break;
        }
    }

    return property;
}

// implement non-const method by means of const method,
// for details, see "Effective C++" (third edition)
GeoSceneProperty* GeoSceneGroup::property( const QString& name )
{
    return const_cast<GeoSceneProperty*>
        ( static_cast<GeoSceneGroup const *>( this )->property( name ));
}

QVector<GeoSceneProperty*> GeoSceneGroup::properties()
{
    return m_properties;
}

QVector<const GeoSceneProperty*> GeoSceneGroup::properties() const
{
    QVector<const GeoSceneProperty*> result;

    foreach ( const GeoSceneProperty *property, m_properties ) {
        result << property;
    }

    return result;
}

QString GeoSceneGroup::name() const
{
    return m_name;
}

}

#include "GeoSceneGroup.moc"
