//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//


// Own
#include "LayerManager.h"

// Qt

// Local dir
#include "GeoPainter.h"
#include "GeoSceneLayer.h"
#include "MarbleLayerInterface.h"
#include "ViewportParams.h"

LayerManager::LayerManager(QObject *parent)
    : QObject(parent)
{
    // Just for initial testing
    m_layerInterfaces = m_pluginManager.layerInterfaces();
}

LayerManager::~LayerManager()
{
}

void LayerManager::renderLayers( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer * layer )
{
    QList<MarbleLayerInterface *> interfaceList = m_pluginManager.layerInterfaces();
    foreach( MarbleLayerInterface * interface,  interfaceList ) {
        interface->render( painter, viewport, "ALWAYS_ON_TOP", layer );
    }

    // Looping a second time through is a quick and dirty way to get 
    // the float items displayed on top:

    foreach( MarbleLayerInterface * interface,  interfaceList ) {
        if ( interface->renderPosition().contains("FLOAT_ITEM") )
            interface->render( painter, viewport, "FLOAT_ITEM", layer );
    }
}

void LayerManager::loadLayers()
{
}

#include "LayerManager.moc"
