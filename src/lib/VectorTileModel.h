/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
 Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
*/

#ifndef MARBLE_VECTORTILEMAPPER_H
#define MARBLE_VECTORTILEMAPPER_H

#include <QObject>
#include <QRunnable>

#include <QCache>

#include "TileId.h"

class QThreadPool;

namespace Marble
{

class GeoDataDocument;
class GeoDataLatLonBox;
class GeoDataTreeModel;
class GeoSceneVectorTile;
class TileLoader;

class TileRunner : public QObject, public QRunnable
{
    Q_OBJECT

public:
    TileRunner( TileLoader *loader, const GeoSceneVectorTile *texture, const TileId &id );
    void run();

Q_SIGNALS:
    void documentLoaded( const TileId &id, GeoDataDocument *document );

private:
    TileLoader *const m_loader;
    const GeoSceneVectorTile *const m_texture;
    const TileId m_id;
};

class VectorTileModel : public QObject
{
    Q_OBJECT

public:
    explicit VectorTileModel( TileLoader *loader, const GeoSceneVectorTile *layer, GeoDataTreeModel *treeModel, QThreadPool *threadPool );

    void setViewport( const GeoDataLatLonBox &bbox, int radius );

    QString name() const;

public Q_SLOTS:
    void updateTile( const TileId &id, GeoDataDocument *document );

    void clear();

Q_SIGNALS:
    void tileCompleted( const TileId &tileId );

private:
    void setViewport( int tileZoomLevel, unsigned int minX, unsigned int minY, unsigned int maxX, unsigned int maxY );

    unsigned int lon2tileX( qreal lon, unsigned int maxTileX );
    unsigned int lat2tileY( qreal lat, unsigned int maxTileY );

private:
    struct CacheDocument
    {
        /** The CacheDocument takes ownership of doc */
        CacheDocument( GeoDataDocument *doc, GeoDataTreeModel *model );

        /** Remove the document from the tree and delete the document */
        ~CacheDocument();

        GeoDataDocument *const m_document;
        GeoDataTreeModel *const m_treeModel;

    private:
        Q_DISABLE_COPY( CacheDocument )
    };

    TileLoader *const m_loader;
    const GeoSceneVectorTile *const m_layer;
    GeoDataTreeModel *const m_treeModel;
    QThreadPool *const m_threadPool;
    int m_tileZoomLevel;
    QCache<TileId, CacheDocument> m_documents;
};

}

#endif // MARBLE_VECTORTILEMAPPER_H
