/*
    SPDX-License-Identifier: LGPL-2.1-or-later

    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>
    SPDX-FileCopyrightText: 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
*/

#ifndef MARBLE_VECTORTILEMODEL_H
#define MARBLE_VECTORTILEMODEL_H

#include <QObject>
#include <QRunnable>

#include <QMap>

#include "GeoDataLatLonBox.h"
#include "TileId.h"

class QThreadPool;

namespace Marble
{

class GeoDataDocument;
class GeoDataTreeModel;
class GeoSceneVectorTileDataset;
class GeoDataObject;
class TileLoader;

class TileRunner : public QObject, public QRunnable
{
    Q_OBJECT

public:
    TileRunner(TileLoader *loader, const GeoSceneVectorTileDataset *texture, const TileId &id);
    void run() override;

Q_SIGNALS:
    void documentLoaded(const TileId &id, GeoDataDocument *document);

private:
    TileLoader *const m_loader;
    const GeoSceneVectorTileDataset *const m_tileDataset;
    const TileId m_id;
};

class VectorTileModel : public QObject
{
    Q_OBJECT

public:
    explicit VectorTileModel(TileLoader *loader, const GeoSceneVectorTileDataset *layer, GeoDataTreeModel *treeModel, QThreadPool *threadPool);

    void setViewport(const GeoDataLatLonBox &bbox);

    QString name() const;

    const GeoSceneVectorTileDataset *layer() const;

    void removeTile(GeoDataDocument *document);

    int tileZoomLevel() const;

    int cachedDocuments() const;

    void reload();

public Q_SLOTS:
    void updateTile(const TileId &id, GeoDataDocument *document);

    void clear();

Q_SIGNALS:
    void tileCompleted(const TileId &tileId);
    void tileAdded(GeoDataDocument *document);
    void tileRemoved(GeoDataDocument *document);

private Q_SLOTS:
    void cleanupTile(GeoDataObject *feature);

private:
    void removeTilesOutOfView(const GeoDataLatLonBox &boundingBox);
    void queryTiles(int tileZoomLevel, const QRect &rect);

private:
    struct CacheDocument {
        /** The CacheDocument takes ownership of doc */
        CacheDocument(GeoDataDocument *doc, VectorTileModel *vectorTileModel, const GeoDataLatLonBox &boundingBox);

        /** Remove the document from the tree and delete the document */
        ~CacheDocument();

        GeoDataLatLonBox latLonBox() const
        {
            return m_boundingBox;
        }

    private:
        Q_DISABLE_COPY(CacheDocument)

        GeoDataDocument *const m_document;
        VectorTileModel *const m_vectorTileModel;
        GeoDataLatLonBox m_boundingBox;
    };

    TileLoader *const m_loader;
    const GeoSceneVectorTileDataset *const m_layer;
    GeoDataTreeModel *const m_treeModel;
    QThreadPool *const m_threadPool;
    int m_tileLoadLevel;
    int m_tileZoomLevel;
    QList<TileId> m_pendingDocuments;
    QList<GeoDataDocument *> m_garbageQueue;
    QMap<TileId, QSharedPointer<CacheDocument>> m_documents;
    bool m_deleteDocumentsLater;
};

}

#endif // MARBLE_VECTORTILEMODEL_H
