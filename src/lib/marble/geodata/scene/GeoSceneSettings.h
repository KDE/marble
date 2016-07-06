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

#ifndef MARBLE_GEOSCENESETTINGS_H
#define MARBLE_GEOSCENESETTINGS_H

#include <QObject>
#include <QVector>

#include <geodata_export.h>

#include "GeoDocument.h"

class QString;

namespace Marble
{

class GeoSceneProperty;
class GeoSceneGroup;

class GeoSceneSettingsPrivate;

/**
 * @short Settings of a GeoScene document.
 */
class GEODATA_EXPORT GeoSceneSettings : public QObject, 
                                        public GeoNode
{
    Q_OBJECT

 public:
    GeoSceneSettings();
    virtual ~GeoSceneSettings();
    virtual const char* nodeType() const;

    /**
     * @brief  Get the availability of a property across groups
     * @param  name  the property name
     * @param  available  availability of the property
     * @return @c true  the property was registered across groups
     *         @c false the property wasn't registered across groups
     */
    bool propertyAvailable( const QString& name, bool& available ) const;

    /**
     * @brief  Set the value of a property across groups
     * @param  name  the property name
     * @param  value  the value of the property
     * @return @c true  the property was found and changed accordingly
     *         @c false the property couldn't be found here
     */
    bool setPropertyValue( const QString& name, bool value );

    /**
     * @brief  Get the value of a property across groups
     * @param  name  the property name
     * @param  value  the value of the property
     * @return @c true  the property was found and returned accordingly
     *         @c false the property couldn't be found in this group
     */
    bool propertyValue( const QString& name, bool& value ) const;

    /**
     * @brief  Get the whole list of properties stored in the settings
     */
    QVector<GeoSceneProperty*> allProperties();

    /**
     * @brief  Get the whole list of properties stored in the settings
     */
    QVector<const GeoSceneProperty*> allProperties() const;

    /**
     * @brief  Add a group to the settings
     * @param  group  the new group
     */
    void addGroup( GeoSceneGroup* group );

    /**
     * @brief  Get a group from the settings
     * @param  name  the name of the group
     */
    const GeoSceneGroup* group( const QString& name ) const;
    GeoSceneGroup* group( const QString& name );

    /**
     * @brief  Add a property to the settings
     * @param  property  the new property
     */
    void addProperty( GeoSceneProperty* property );

    /**
     * @brief  Get a property from the settings
     * @param  name  the name of the property
     */
    const GeoSceneProperty* property( const QString& name ) const;
    GeoSceneProperty* property( const QString& name );

    /**
     * @brief  Get the properties that are categorized into groups
     *
     * NOTE: If you want all the properties distributed among groups 
     *       then please use:  QVector<GeoSceneProperty*> allProperties().
     */
    QVector<GeoSceneProperty*> rootProperties();

 Q_SIGNALS:
    void valueChanged( const QString&, bool );

 private:
    Q_DISABLE_COPY( GeoSceneSettings )
    GeoSceneSettingsPrivate * const d;
};

}

#endif
