/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOSCENEGROUP_H
#define MARBLE_GEOSCENEGROUP_H

#include <QList>
#include <QObject>
#include <QString>

#include "GeoDocument.h"

namespace Marble
{

class GeoSceneProperty;

/**
 * @short Group inside the settings of a GeoScene document.
 */

class GeoSceneGroup : public QObject, public GeoNode
{
    Q_OBJECT

public:
    explicit GeoSceneGroup(const QString &name);
    ~GeoSceneGroup() override;

    /**
     * @brief  Get the availability of a property in this group
     * @param  name  the property name
     * @param  available  availability of the property
     * @return @c true  the property was registered in this group
     *         @c false the property wasn't registered in this group
     */
    bool propertyAvailable(const QString &name, bool &available) const;

    /**
     * @brief  Set the value of a property in this group
     * @param  name  the property name
     * @param  value  the value of the property
     * @return @c true  the property was found and changed accordingly
     *         @c false the property couldn't be found in this group
     */
    bool setPropertyValue(const QString &name, bool value);

    /**
     * @brief  Get the value of a property in this group
     * @param  name  the property name
     * @param  value  the value of the property
     * @return @c true  the property was found and returned accordingly
     *         @c false the property couldn't be found in this group
     */
    bool propertyValue(const QString &name, bool &value) const;

    /**
     * @brief  Add a property to this setting group
     * @param  property  the new property
     */
    void addProperty(GeoSceneProperty *);
    const GeoSceneProperty *property(const QString &name) const;
    GeoSceneProperty *property(const QString &name);
    QList<GeoSceneProperty *> properties();
    QList<const GeoSceneProperty *> properties() const;

    QString name() const;

    const char *nodeType() const override;

Q_SIGNALS:
    void valueChanged(const QString &, bool);

private:
    Q_DISABLE_COPY(GeoSceneGroup)

    /// The vector holding all the properties in this settings group.
    QList<GeoSceneProperty *> m_properties;

    QString m_name;
};

}

#endif
