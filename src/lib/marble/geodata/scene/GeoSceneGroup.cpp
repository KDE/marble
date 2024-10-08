/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GeoSceneGroup.h"

#include "GeoSceneProperty.h"
#include "GeoSceneTypes.h"

namespace Marble
{

GeoSceneGroup::GeoSceneGroup(const QString &name)
    : m_name(name)
{
}

GeoSceneGroup::~GeoSceneGroup()
{
    qDeleteAll(m_properties);
}

bool GeoSceneGroup::propertyAvailable(const QString &name, bool &available) const
{
    QList<GeoSceneProperty *>::const_iterator it = m_properties.constBegin();
    QList<GeoSceneProperty *>::const_iterator end = m_properties.constEnd();
    for (; it != end; ++it) {
        if ((*it)->name() == name) {
            available = (*it)->available();
            return true;
        }
    }

    available = false;

    return false;
}

bool GeoSceneGroup::setPropertyValue(const QString &name, bool value)
{
    QList<GeoSceneProperty *>::const_iterator it = m_properties.constBegin();
    QList<GeoSceneProperty *>::const_iterator end = m_properties.constEnd();
    for (; it != end; ++it) {
        if ((*it)->name() == name) {
            (*it)->setValue(value);
            Q_EMIT valueChanged(name, value);
            return true;
        }
    }

    return false;
}

bool GeoSceneGroup::propertyValue(const QString &name, bool &value) const
{
    QList<GeoSceneProperty *>::const_iterator it = m_properties.constBegin();
    QList<GeoSceneProperty *>::const_iterator end = m_properties.constEnd();
    for (; it != end; ++it) {
        if ((*it)->name() == name) {
            value = (*it)->value();
            return true;
        }
    }

    value = false;

    return false;
}

void GeoSceneGroup::addProperty(GeoSceneProperty *property)
{
    Q_ASSERT(property);
    if (!property) {
        return;
    }

    // Remove any property that has the same name
    QList<GeoSceneProperty *>::iterator it = m_properties.begin();
    while (it != m_properties.end()) {
        GeoSceneProperty *currentProperty = *it;
        if (currentProperty->name() == property->name()) {
            delete currentProperty;
            m_properties.erase(it);
            break;
        } else {
            ++it;
        }
    }

    m_properties.append(property);

    // Establish connection to the outside, e.g. the LegendBrowser
    connect(property, SIGNAL(valueChanged(QString, bool)), SIGNAL(valueChanged(QString, bool)));
    Q_EMIT valueChanged(property->name(), property->value());
}

const GeoSceneProperty *GeoSceneGroup::property(const QString &name) const
{
    GeoSceneProperty *property = nullptr;

    QList<GeoSceneProperty *>::const_iterator it = m_properties.constBegin();
    QList<GeoSceneProperty *>::const_iterator end = m_properties.constEnd();
    for (; it != end; ++it) {
        if ((*it)->name() == name) {
            property = *it;
            break;
        }
    }

    return property;
}

// implement non-const method by means of const method,
// for details, see "Effective C++" (third edition)
GeoSceneProperty *GeoSceneGroup::property(const QString &name)
{
    return const_cast<GeoSceneProperty *>(static_cast<GeoSceneGroup const *>(this)->property(name));
}

QList<GeoSceneProperty *> GeoSceneGroup::properties()
{
    return m_properties;
}

QList<const GeoSceneProperty *> GeoSceneGroup::properties() const
{
    QList<const GeoSceneProperty *> result;
    result.reserve(m_properties.size());

    for (const GeoSceneProperty *property : m_properties) {
        result << property;
    }

    return result;
}

QString GeoSceneGroup::name() const
{
    return m_name;
}

const char *GeoSceneGroup::nodeType() const
{
    return GeoSceneTypes::GeoSceneGroupType;
}

}

#include "moc_GeoSceneGroup.cpp"
