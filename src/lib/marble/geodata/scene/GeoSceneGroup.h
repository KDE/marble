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

#ifndef MARBLE_GEOSCENEGROUP_H
#define MARBLE_GEOSCENEGROUP_H

#include <QObject>
#include <QString>
#include <QVector>

#include "GeoDocument.h"

namespace Marble
{

class GeoSceneProperty;

/**
 * @short Group inside the settings of a GeoScene document.
 */

class GeoSceneGroup : public QObject,
                      public GeoNode
{
    Q_OBJECT

 public:
    explicit GeoSceneGroup( const QString& name );
    ~GeoSceneGroup();

    /**
     * @brief  Get the availability of a property in this group
     * @param  name  the property name
     * @param  available  availability of the property
     * @return @c true  the property was registered in this group
     *         @c false the property wasn't registered in this group
     */
    bool propertyAvailable( const QString& name, bool& available ) const;

    /**
     * @brief  Set the value of a property in this group
     * @param  name  the property name
     * @param  value  the value of the property
     * @return @c true  the property was found and changed accordingly
     *         @c false the property couldn't be found in this group
     */
    bool setPropertyValue( const QString& name, bool value );

    /**
     * @brief  Get the value of a property in this group
     * @param  name  the property name
     * @param  value  the value of the property
     * @return @c true  the property was found and returned accordingly
     *         @c false the property couldn't be found in this group
     */
    bool propertyValue( const QString& name, bool& value ) const;

    /**
     * @brief  Add a property to this setting group
     * @param  property  the new property
     */
    void addProperty(GeoSceneProperty*);
    const GeoSceneProperty* property( const QString& name ) const;
    GeoSceneProperty* property( const QString& name );
    QVector<GeoSceneProperty*> properties();
    QVector<const GeoSceneProperty*> properties() const;

    QString name() const;

 Q_SIGNALS:
    void valueChanged( const QString&, bool );

 private:
    Q_DISABLE_COPY( GeoSceneGroup )

    /// The vector holding all the properties in this settings group.
    QVector<GeoSceneProperty*> m_properties;

    QString m_name;
};

}

#endif
