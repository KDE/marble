/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2008      Patrick Spendrin  <ps_ml@gmx.de>
 Copyright 2010      Thibaut Gridel  <tgridel@free.fr>
 Copyright 2012      Ander Pijoan <ander.pijoan@deusto.es>
 Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
*/

#ifndef MARBLE_VECTORTILELAYER_H
#define MARBLE_VECTORTILELAYER_H

#include "LayerInterface.h"
#include <QObject>

#include "MarbleGlobal.h"

namespace Marble
{

class GeoPainter;
class GeoDataDocument;
class GeoSceneGroup;
class GeoSceneVectorTileDataset;
class GeoDataTreeModel;
class PluginManager;
class HttpDownloadManager;
class ViewportParams;
class TileId;

class VectorTileLayer : public QObject, public LayerInterface
{
    Q_OBJECT

 public:
    VectorTileLayer( HttpDownloadManager *downloadManager,
                  const PluginManager *pluginManager,
                  GeoDataTreeModel *treeModel);

    ~VectorTileLayer();

    QStringList renderPosition() const;

    RenderState renderState() const;

    int tileZoomLevel() const;

    QString runtimeTrace() const;

    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString &renderPos = QLatin1String("NONE"),
                 GeoSceneLayer *layer = 0 );

Q_SIGNALS:
    void tileLevelChanged(int tileLevel);

 public Q_SLOTS:
    void setMapTheme( const QVector<const GeoSceneVectorTileDataset *> &textures, const GeoSceneGroup *textureLayerSettings );

    void reset();

 private:
    Q_PRIVATE_SLOT( d, void updateTextureLayers() )
    Q_PRIVATE_SLOT( d, void updateTile(const TileId &tileId, GeoDataDocument* document) )


 private:
    class Private;
    Private *const d;

};

}

#endif // MARBLE_VECTORTILELAYER_H
