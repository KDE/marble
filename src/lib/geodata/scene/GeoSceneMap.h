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

#ifndef MARBLE_GEOSCENEMAP_H
#define MARBLE_GEOSCENEMAP_H

#include <QtGui/QColor>
#include <QtCore/QString>
#include <QtCore/QVector>

#include <geodata_export.h>

#include "GeoDocument.h"

namespace Marble
{

class GeoSceneLayer;
class GeoSceneFilter;

class GeoSceneMapPrivate;

/**
 * @short Map layer structure of a GeoScene document.
 */
class GEODATA_EXPORT GeoSceneMap : public GeoNode
{
 public:
    GeoSceneMap();
    ~GeoSceneMap();
    virtual const char* nodeType() const;

    QColor backgroundColor() const;
    void setBackgroundColor( const QColor& );

    QColor labelColor() const;
    void setLabelColor( const QColor& );

    /**
     * @brief  Add a new layer to the map
     * @param  section  The new layer
     */
    void addLayer( GeoSceneLayer* );

    /**
     * @brief  Return a layer by its name
     * @param  name  The name of the layer
     * @return A pointer to the layer request by its name
     */
    GeoSceneLayer* layer( const QString& name );
    const GeoSceneLayer* layer( const QString& name ) const;

    /**
     * @brief  Return all layers
     * @return A vector that contains pointers to all available layers
     */
    QVector<GeoSceneLayer*> layers() const;

    /**
     * @brief  Add a new filter to the map
     * @param  section  The new filter
     */
    void addFilter( GeoSceneFilter* );

    /**
     * @brief  Return a filter by its name
     * @param  name  The name of the filter
     * @return A pointer to the filter request by its name
     */
    GeoSceneFilter* filter( const QString& name );

    /**
     * @brief  Return all filters
     * @return A vector that contains pointers to all available filters
     */
    QVector<GeoSceneFilter*> filters() const;

    /**
     * @brief  Checks for valid layers that contain texture data
     * @return Whether a texture layer got created internally 
     *
     * NOTE: The existence of the file(s) that contain the actual data  
     *       still needs to get checked at runtime!
     */
    bool hasTextureLayers() const;

    /**
     * @brief  Checks for valid layers that contain vector data
     * @return Whether a vector layer got created internally 
     *
     * NOTE: The existence of the file(s) that contain the actual data  
     *       still needs to get checked at runtime!
     */
    bool hasVectorLayers() const;

 private:
    Q_DISABLE_COPY( GeoSceneMap )
    GeoSceneMapPrivate * const d;
};

}

#endif
