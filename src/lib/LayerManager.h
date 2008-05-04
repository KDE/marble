//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2008 Torsten Rahn <tackat@kde.org>"
//


#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

// Qt
#include <QtCore/QObject>

// Local dir
#include "PluginManager.h"

class GeoPainter;
class GeoSceneLayer;
class ViewportParams;

/**
 * @short The class that handles Marble's DGML layers.
 *
 */

class LayerManager : public QObject
{
    Q_OBJECT

 public:
    LayerManager(QObject *parent = 0);
    ~LayerManager();

    void renderLayers( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer * layer = 0 );

 public Q_SLOTS:
    void loadLayers();

 private:
    PluginManager m_pluginManager;

    QList<MarbleLayerInterface *> m_layerInterfaces;
};


#endif // LAYERMANAGER_H
