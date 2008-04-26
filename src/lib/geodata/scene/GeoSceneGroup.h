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

#ifndef GEOSCENEGROUP_H
#define GEOSCENEGROUP_H

#include <QtCore/QVector>

#include "GeoDocument.h"

class GeoSceneProperty;

/**
 * @short Group inside the settings of a GeoScene document.
 */

class GeoSceneGroup : public GeoNode {
 public:
    GeoSceneGroup( const QString& name );
    ~GeoSceneGroup();

    /**
     * @brief  Add a property to this setting group
     * @param  property  the new property
     */
    void addProperty(GeoSceneProperty*);
    GeoSceneProperty* property(const QString&);
    QVector<GeoSceneProperty*> properties() const;

    QString name() const;

 protected:
    /// The vector holding all the properties in this settings group.
    QVector<GeoSceneProperty*> m_properties;

    QString m_name;
};


#endif // GEOSCENEGROUP_H
