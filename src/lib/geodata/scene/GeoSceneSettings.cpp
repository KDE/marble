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

#include "MarbleDebug.h"

#include "GeoSceneProperty.h"
#include "GeoSceneGroup.h"
#include "GeoSceneTypes.h"

namespace Marble
{

class GeoSceneSettingsPrivate
{
  public:
    ~GeoSceneSettingsPrivate()
    {
        qDeleteAll(m_properties);
        qDeleteAll(m_groups);
    }

    /// The hash table holding all the properties in the settings.
    QVector<GeoSceneProperty*> m_properties;
    QVector<GeoSceneGroup*> m_groups;
    
    const char* nodeType() const
    {
        return GeoSceneTypes::GeoSceneSettingsType;
    }
};


GeoSceneSettings::GeoSceneSettings()
    : d( new GeoSceneSettingsPrivate )
{
}

GeoSceneSettings::~GeoSceneSettings()
{
    delete d;
}

const char* GeoSceneSettings::nodeType() const
{
    return d->nodeType();
}

bool GeoSceneSettings::propertyAvailable( const QString& name, bool& available ) const
{
    QVector<GeoSceneProperty*>::const_iterator it = d->m_properties.constBegin();
    QVector<GeoSceneProperty*>::const_iterator propEnd = d->m_properties.constEnd();
    for (; it != propEnd; ++it) {
        if ( (*it)->name() == name ) {
            available = (*it)->available();
            return true;
        }
    }

    QVector<GeoSceneGroup*>::const_iterator itGroup = d->m_groups.constBegin();
    QVector<GeoSceneGroup*>::const_iterator groupEnd = d->m_groups.constEnd();
    for (; itGroup != groupEnd; ++itGroup) {
        bool success = (*itGroup)->propertyAvailable( name, available );
        if ( success ) {
            return true;
        }
    }

    available = false;

    return false;
}

bool GeoSceneSettings::setPropertyValue( const QString& name, bool value )
{
    mDebug() << "GeoSceneSettings: Property " << name << "to" << value; 
    
    QVector<GeoSceneProperty*>::const_iterator it = d->m_properties.constBegin();
    QVector<GeoSceneProperty*>::const_iterator propEnd = d->m_properties.constEnd();
    for (; it != propEnd; ++it) {
        if ( (*it)->name() == name ) {
            (*it)->setValue( value );
            return true;
        }
    }

    QVector<GeoSceneGroup*>::const_iterator itGroup = d->m_groups.constBegin();
    QVector<GeoSceneGroup*>::const_iterator groupEnd = d->m_groups.constEnd();
    for (; itGroup != groupEnd; ++itGroup) {
        bool success = (*itGroup)->setPropertyValue( name, value );
        if ( success ) {
            return true;
        }
    }

    return false;
}

bool GeoSceneSettings::propertyValue( const QString& name, bool& value ) const
{
    QVector<GeoSceneProperty*>::const_iterator it = d->m_properties.constBegin();
    QVector<GeoSceneProperty*>::const_iterator propEnd = d->m_properties.constEnd();
    for (; it != propEnd; ++it) {
        if ( (*it)->name() == name ) {
            value = (*it)->value();
            return true;
        }
    }

    QVector<GeoSceneGroup*>::const_iterator itGroup = d->m_groups.constBegin();
    QVector<GeoSceneGroup*>::const_iterator groupEnd = d->m_groups.constEnd();
    for (; itGroup != groupEnd; ++itGroup) {
        bool success = (*itGroup)->propertyValue( name, value );
        if ( success ) {
            return true;
        }
    }

    value = false;

    return false;
}

QVector<GeoSceneProperty*> GeoSceneSettings::allProperties()
{
    QVector<GeoSceneProperty*> allProperties;

    QVector<GeoSceneGroup*>::const_iterator itGroup = d->m_groups.constBegin();
    QVector<GeoSceneGroup*>::const_iterator groupEnd = d->m_groups.constEnd();
    for (; itGroup != groupEnd; ++itGroup) {
        allProperties << (*itGroup)->properties();
    }

    allProperties << d->m_properties;

    return allProperties;
}

QVector<const GeoSceneProperty*> GeoSceneSettings::allProperties() const
{
    QVector<const GeoSceneProperty*> allProperties;

    QVector<GeoSceneGroup*>::const_iterator itGroup = d->m_groups.constBegin();
    QVector<GeoSceneGroup*>::const_iterator groupEnd = d->m_groups.constEnd();
    for (; itGroup != groupEnd; ++itGroup) {
        allProperties << const_cast<const GeoSceneGroup*>(*itGroup)->properties();
    }

    foreach ( const GeoSceneProperty *property, d->m_properties ) {
        allProperties << property;
    }

    return allProperties;
}

void GeoSceneSettings::addGroup( GeoSceneGroup* group )
{
    // Remove any property that has the same name
    QVector<GeoSceneGroup*>::iterator it = d->m_groups.begin();
    while (it != d->m_groups.end()) {
        GeoSceneGroup* currentGroup = *it;
        if ( currentGroup->name() == group->name() ) {
            delete currentGroup;
            it = d->m_groups.erase(it);
            break;
        }
        else {
            ++it;
        }
     }

    if ( group ) {
        d->m_groups.append( group );

        // Establish connection to the outside, e.g. the LegendBrowser
        connect ( group, SIGNAL( valueChanged( QString, bool ) ), 
                         SIGNAL( valueChanged( QString, bool ) ) );
    }
}

const GeoSceneGroup* GeoSceneSettings::group( const QString& name ) const
{
    GeoSceneGroup* group = 0;

    QVector<GeoSceneGroup*>::const_iterator it = d->m_groups.constBegin();
    QVector<GeoSceneGroup*>::const_iterator groupEnd = d->m_groups.constEnd();
    for (; it != groupEnd; ++it) {
        if ( (*it)->name() == name ) {
            group = *it;
            break;
        }
    }

    return group;
}

// implement non-const method by means of const method,
// for details, see "Effective C++" (third edition)
GeoSceneGroup* GeoSceneSettings::group( const QString& name )
{
    return const_cast<GeoSceneGroup*>( static_cast<GeoSceneSettings const *>( this )->group( name ));
}

void GeoSceneSettings::addProperty( GeoSceneProperty* property )
{
    // Remove any property that has the same name
    QVector<GeoSceneProperty*>::iterator it = d->m_properties.begin();
    while (it != d->m_properties.end()) {
        GeoSceneProperty* currentProperty = *it;
        if ( currentProperty->name() == property->name() ) {
            delete currentProperty;
            it = d->m_properties.erase(it);
            break;
        }
        else {
            ++it;
        }
     }

    if ( property ) {
        d->m_properties.append( property );

        // Establish connection to the outside, e.g. the LegendBrowser
        connect ( property, SIGNAL( valueChanged( QString, bool ) ), 
                            SIGNAL( valueChanged( QString, bool ) ) );
        emit valueChanged( property->name(), property->value() );
    }
}

const GeoSceneProperty* GeoSceneSettings::property( const QString& name ) const
{
    GeoSceneProperty* property = 0;

    QVector<GeoSceneProperty*>::const_iterator it = d->m_properties.constBegin();
    QVector<GeoSceneProperty*>::const_iterator propEnd = d->m_properties.constEnd();
    for (; it != propEnd; ++it) {
        if ( (*it)->name() == name ) {
            property = *it;
            break;
        }
    }

    return property;
}

// implement non-const method by means of const method,
// for details, see "Effective C++" (third edition)
GeoSceneProperty* GeoSceneSettings::property( const QString& name )
{
    return const_cast<GeoSceneProperty*>
        ( static_cast<GeoSceneSettings const *>( this )->property( name ));
}

QVector<GeoSceneProperty*> GeoSceneSettings::rootProperties()
{
    return d->m_properties;
}

}

#include "GeoSceneSettings.moc"
