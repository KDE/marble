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

class GeoSceneMapPrivate
{
  public:
    GeoSceneMapPrivate()
        : m_backgroundColor( "" )
    {
    }

    ~GeoSceneMapPrivate()
    {
        qDeleteAll( m_layers );
    }

    /// The vector holding all the sections in the legend.
    /// (We want to preserve the order and don't care 
    /// much about speed here), so we don't use a hash
    QVector<GeoSceneLayer*> m_layers;

    QString m_backgroundColor;
};


GeoSceneMap::GeoSceneMap()
    : d ( new GeoSceneMapPrivate )
{
    /* NOOP */
}

GeoSceneMap::~GeoSceneMap()
{
    delete d;
}

void GeoSceneMap::addLayer( GeoSceneLayer* layer )
{
    // Remove any layer that has the same name
    QVector<GeoSceneLayer*>::iterator it = d->m_layers.begin();
    while (it != d->m_layers.end()) {
        GeoSceneLayer* currentLayer = *it;
        if ( currentLayer->name() == layer->name() ) {
            delete currentLayer;
            it = d->m_layers.erase(it);
        }
        else {
            ++it;
        }
     }

    if ( layer ) {
        d->m_layers.append( layer );
    }
}

GeoSceneLayer* GeoSceneMap::layer( const QString& name )
{
    GeoSceneLayer* layer = 0;

    QVector<GeoSceneLayer*>::const_iterator it = d->m_layers.begin();
    for (it = d->m_layers.begin(); it != d->m_layers.end(); ++it) {
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
    return d->m_layers;
}

bool GeoSceneMap::hasTextureLayers() const
{
    QVector<GeoSceneLayer*>::const_iterator it = d->m_layers.begin();
    for (it = d->m_layers.begin(); it != d->m_layers.end(); ++it) {
        if ( (*it)->backend() == dgmlValue_texture && (*it)->datasets().count() > 0 )
            return true;
    }

    return false;
}

bool GeoSceneMap::hasVectorLayers() const
{
    QVector<GeoSceneLayer*>::const_iterator it = d->m_layers.begin();
    for (it = d->m_layers.begin(); it != d->m_layers.end(); ++it) {
        if ( (*it)->backend() == dgmlValue_vector && (*it)->datasets().count() > 0 )
            return true;
    }

    return false;
}

QString GeoSceneMap::backgroundColor() const
{
    return d->m_backgroundColor;
}

void GeoSceneMap::setBackgroundColor( const QString& backgroundColor )
{
    d->m_backgroundColor = backgroundColor;
}
