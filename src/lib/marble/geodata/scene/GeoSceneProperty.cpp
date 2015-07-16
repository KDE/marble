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

#include "GeoSceneProperty.h"

#include "MarbleDebug.h"
#include "GeoSceneTypes.h"

namespace Marble
{

GeoSceneProperty::GeoSceneProperty( const QString& name )
    : m_name( name ),
      m_available( false ),
      m_defaultValue( false ),
      m_value( false )
{
}
 
const char* GeoSceneProperty::nodeType() const
{
    return GeoSceneTypes::GeoScenePropertyType;
}

QString GeoSceneProperty::name() const
{
    return m_name;
}

bool GeoSceneProperty::available() const
{
    return m_available;
}

void GeoSceneProperty::setAvailable( bool available )
{
    m_available = available;
}

bool GeoSceneProperty::defaultValue() const
{
    return m_defaultValue;
}

void GeoSceneProperty::setDefaultValue( bool defaultValue )
{
    m_defaultValue = defaultValue;
    setValue( defaultValue );
}

bool GeoSceneProperty::value() const
{
    return m_value;
}

void GeoSceneProperty::setValue( bool value )
{
    if ( m_value == value ) 
        return;

    m_value = value;
//    mDebug() << "GeoSceneProperty: Setting " << m_name << "to" << m_value; 
    emit valueChanged( m_name, m_value );
}

}

#include "moc_GeoSceneProperty.cpp"
