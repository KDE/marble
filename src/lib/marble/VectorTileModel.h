/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
 Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
*/

#ifndef MARBLE_VECTORTILEMODEL_H
#define MARBLE_VECTORTILEMODEL_H

#include <QObject>
#include <QRunnable>

#include <QMap>

#include "TileId.h"
#include "GeoDataLatLonBox.h"

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
    TileRunner( TileLoader *loader, const GeoSceneVectorTileDataset *texture, const TileId &id );
    void run();

Q_SIGNALS:
    void documentLoaded( const TileId &id, GeoDataDocument *document );

private:
    TileLoader *const m_loader;
    const GeoSceneVectorTileDataset *const m_texture;
    const TileId m_id;
};

class VectorTileModel : public QObject
{
    Q_OBJECT

public:
    explicit VectorTileModel( TileLoader *loader, const GeoSceneVectorTileDataset *layer, GeoDataTreeModel *treeModel, QThreadPool *threadPool );

    void setViewport( const GeoDataLatLonBox &bbox, int radius );

    QString name() const;

    void removeTile(GeoDataDocument* document);

    int tileZoomLevel() const;

    int cachedDocuments() const;

public Q_SLOTS:
    void updateTile( const TileId &id, GeoDataDocument *document );

    void clear();

Q_SIGNALS:
    void tileCompleted( const TileId &tileId );
    void tileAdded(GeoDataDocument *document);
    void tileRemoved(GeoDataDocument *document);

private Q_SLOTS:
    void cleanupTile(GeoDataObject* feature);

private:
    void removeTilesOutOfView(const GeoDataLatLonBox &boundingBox);
    void queryTiles( int tileZoomLevel, unsigned int minX, unsigned int minY, unsigned int maxX, unsigned int maxY );

private:
    struct CacheDocument
    {
        /** The CacheDocument takes ownership of doc */
        CacheDocument(GeoDataDocument *doc, VectorTileModel* vectorTileModel, const GeoDataLatLonBox &boundingBox);

        /** Remove the document from the tree and delete the document */
        ~CacheDocument();

        GeoDataDocument *const m_document;
        VectorTileModel *m_vectorTileModel;
        GeoDataLatLonBox m_boundingBox;

    private:
        Q_DISABLE_COPY( CacheDocument )
    };

    TileLoader *const m_loader;
    const GeoSceneVectorTileDataset *const m_layer;
    GeoDataTreeModel *const m_treeModel;
    QThreadPool *const m_threadPool;
    int m_tileLoadLevel;
    int m_tileZoomLevel;
    QList<TileId> m_pendingDocuments;
    QList<GeoDataDocument*> m_garbageQueue;
    QMap<TileId, QSharedPointer<CacheDocument> > m_documents;
    bool m_deleteDocumentsLater;
};

}

#endif // MARBLE_VECTORTILEMODEL_H
