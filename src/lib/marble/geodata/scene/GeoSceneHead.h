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

#ifndef MARBLE_GEOSCENEHEAD_H
#define MARBLE_GEOSCENEHEAD_H

#include "GeoDocument.h"

#include <geodata_export.h>

#include <QtGlobal>

class QString;

namespace Marble
{

class GeoSceneIcon;
class GeoSceneZoom;
class GeoSceneLicense;

class GeoSceneHeadPrivate;

/**
 * @short General properties and identifiers of a GeoScene document.
 */
class GEODATA_EXPORT GeoSceneHead : public GeoNode
{
 public:
    GeoSceneHead();
    ~GeoSceneHead();
    
    virtual const char* nodeType() const;

    QString name() const;
    void setName( const QString& name );
    QString target() const;
    void setTarget( const QString& target );
    QString theme() const;
    void setTheme( const QString& theme );

    /** Planet radius, 0.0 if unknown */
    qreal radius() const;
    void setRadius( qreal radius );

    QString mapThemeId() const;

    QString description() const;
    void setDescription( const QString& );

    bool visible() const;
    void setVisible( bool visible );

    const GeoSceneZoom* zoom() const;
    GeoSceneZoom* zoom();

    const GeoSceneIcon* icon() const;
    GeoSceneIcon* icon();

    const GeoSceneLicense* license() const;
    GeoSceneLicense* license();


 private:
    Q_DISABLE_COPY( GeoSceneHead )
    GeoSceneHeadPrivate * const d;
};

}

#endif
