/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GeoSceneProperty.h"

#include "GeoSceneTypes.h"

namespace Marble
{

GeoSceneProperty::GeoSceneProperty(const QString &name)
    : m_name(name)
    , m_available(false)
    , m_defaultValue(false)
    , m_value(false)
{
}

const char *GeoSceneProperty::nodeType() const
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

void GeoSceneProperty::setAvailable(bool available)
{
    m_available = available;
}

bool GeoSceneProperty::defaultValue() const
{
    return m_defaultValue;
}

void GeoSceneProperty::setDefaultValue(bool defaultValue)
{
    m_defaultValue = defaultValue;
    setValue(defaultValue);
}

bool GeoSceneProperty::value() const
{
    return m_value;
}

void GeoSceneProperty::setValue(bool value)
{
    if (m_value == value)
        return;

    m_value = value;
    //    mDebug() << "GeoSceneProperty: Setting " << m_name << "to" << m_value;
    Q_EMIT valueChanged(m_name, m_value);
}

}

#include "moc_GeoSceneProperty.cpp"
