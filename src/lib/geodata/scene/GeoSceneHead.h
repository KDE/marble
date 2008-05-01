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

#ifndef GEOSCENEHEAD_H
#define GEOSCENEHEAD_H

#include <QtCore/QString>

#include <geodata_export.h>

#include "GeoDocument.h"
#include "GeoSceneIcon.h"
#include "GeoSceneZoom.h"

/**
 * @short General properties and identifiers of a GeoScene document.
 */
class GEODATA_EXPORT GeoSceneHead : public GeoNode {
 public:
    GeoSceneHead();
    ~GeoSceneHead();

    const QString name() const;
    void setName( const QString& name );
    const QString target() const;
    void setTarget( const QString& target );
    const QString theme() const;
    void setTheme( const QString& theme );

    const QString mapThemeId() const;

    const QString description() const;
    void setDescription( const QString& );

    bool visible() const;
    void setVisible( bool visible );

    GeoSceneZoom* zoom() const;
    GeoSceneIcon* icon() const;

 protected:
    GeoSceneZoom* m_zoom;
    GeoSceneIcon* m_icon;

    QString m_name;
    QString m_target;
    QString m_theme;
    QString m_description;

    bool    m_visible;
};


#endif // GEOSCENEHEAD_H
