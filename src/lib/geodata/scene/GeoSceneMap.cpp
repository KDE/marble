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

#include "GeoSceneMap.h"

#include "GeoSceneLayer.h"
#include "DgmlAuxillaryDictionary.h"

using namespace GeoSceneAuxillaryDictionary;


GeoSceneMap::GeoSceneMap()
    : m_backgroundColor( "" )
{
    /* NOOP */
}

GeoSceneMap::~GeoSceneMap()
{
    qDeleteAll( m_layers );
}

void GeoSceneMap::addLayer( GeoSceneLayer* layer )
{
    // Remove any layer that has the same name
    QVector<GeoSceneLayer*>::iterator it = m_layers.begin();
    while (it != m_layers.end()) {
        GeoSceneLayer* currentLayer = *it;
        if ( currentLayer->name() == layer->name() ) {
            delete currentLayer;
            it = m_layers.erase(it);
        }
        else {
            ++it;
        }
     }

    if ( layer ) {
        m_layers.append( layer );
    }
}

GeoSceneLayer* GeoSceneMap::layer( const QString& name )
{
    GeoSceneLayer* layer = 0;

    QVector<GeoSceneLayer*>::const_iterator it = m_layers.begin();
    for (it = m_layers.begin(); it != m_layers.end(); ++it) {
        if ( (*it)->name() == name )
            layer = *it;
    }

    if ( layer ) {
        Q_ASSERT(layer->name() == name);
        return layer;
    }

    layer = new GeoSceneLayer( name );
    addLayer( layer );

    return layer;
}

QVector<GeoSceneLayer*> GeoSceneMap::layers() const
{
    return m_layers;
}

bool GeoSceneMap::hasTextureLayers() const
{
    QVector<GeoSceneLayer*>::const_iterator it = m_layers.begin();
    for (it = m_layers.begin(); it != m_layers.end(); ++it) {
        if ( (*it)->backend() == dgmlValue_texture && (*it)->datasets().count() > 0 )
            return true;
    }

    return false;
}

bool GeoSceneMap::hasVectorLayers() const
{
    QVector<GeoSceneLayer*>::const_iterator it = m_layers.begin();
    for (it = m_layers.begin(); it != m_layers.end(); ++it) {
        if ( (*it)->backend() == dgmlValue_vector && (*it)->datasets().count() > 0 )
            return true;
    }

    return false;
}

QString GeoSceneMap::backgroundColor() const
{
    return m_backgroundColor;
}

void GeoSceneMap::setBackgroundColor( const QString& backgroundColor )
{
    m_backgroundColor = backgroundColor;
}
