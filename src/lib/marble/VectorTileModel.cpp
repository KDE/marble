/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
 Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
*/

#include "VectorTileModel.h"

#include "GeoDataDocument.h"
#include "GeoDataLatLonBox.h"
#include "GeoDataTreeModel.h"
#include "GeoDataTypes.h"
#include "GeoSceneVectorTileDataset.h"
#include "MarbleGlobal.h"
#include "MarbleDebug.h"
#include "MathHelper.h"
#include "TileId.h"
#include "TileLoader.h"

#include <qmath.h>
#include <QThreadPool>

using namespace Marble;

TileRunner::TileRunner( TileLoader *loader, const GeoSceneVectorTileDataset *texture, const TileId &id ) :
    m_loader( loader ),
    m_texture( texture ),
    m_id( id )
{
}

void TileRunner::run()
{
    GeoDataDocument *const document = m_loader->loadTileVectorData( m_texture, m_id, DownloadBrowse );

    emit documentLoaded( m_id, document );
}

VectorTileModel::CacheDocument::CacheDocument(GeoDataDocument *doc, VectorTileModel *vectorTileModel, const GeoDataLatLonBox &boundingBox) :
    m_document( doc ),
    m_vectorTileModel(vectorTileModel),
    m_boundingBox(boundingBox)
{
    // nothing to do
}

VectorTileModel::CacheDocument::~CacheDocument()
{
    m_vectorTileModel->removeTile(m_document);
}

VectorTileModel::VectorTileModel( TileLoader *loader, const GeoSceneVectorTileDataset *layer, GeoDataTreeModel *treeModel, QThreadPool *threadPool ) :
    m_loader( loader ),
    m_layer( layer ),
    m_treeModel( treeModel ),
    m_threadPool( threadPool ),
    m_tileZoomLevel( -1 )
{
    connect(this, SIGNAL(tileAdded(GeoDataDocument*)), treeModel, SLOT(addDocument(GeoDataDocument*)) );
    connect(this, SIGNAL(tileRemoved(GeoDataDocument*)), treeModel, SLOT(removeDocument(GeoDataDocument*)) );
    connect(treeModel, SIGNAL(removed(GeoDataObject*)), this, SLOT(cleanupTile(GeoDataObject*)) );
}

void VectorTileModel::setViewport( const GeoDataLatLonBox &bbox, int radius )
{
    // choose the smaller dimension for selecting the tile level, leading to higher-resolution results
    const int levelZeroWidth = m_layer->tileSize().width() * m_layer->levelZeroColumns();
    const int levelZeroHight = m_layer->tileSize().height() * m_layer->levelZeroRows();
    const int levelZeroMinDimension = qMin( levelZeroWidth, levelZeroHight );

    qreal linearLevel = ( 4.0 * (qreal)( radius ) / (qreal)( levelZeroMinDimension ) );

    if ( linearLevel < 1.0 )
        linearLevel = 1.0; // Dirty fix for invalid entry linearLevel

    // As our tile resolution doubles with each level we calculate
    // the tile level from tilesize and the globe radius via log(2)

    qreal tileLevelF = qLn( linearLevel ) / qLn( 2.0 );
    int tileZoomLevel = (int)( tileLevelF * 1.00001 );
    // snap to the sharper tile level a tiny bit earlier
    // to work around rounding errors when the radius
    // roughly equals the global texture width


    if ( tileZoomLevel > m_layer->maximumTileLevel() )
        tileZoomLevel = m_layer->maximumTileLevel();

    // if zoom level has changed, empty vectortile cache
    if ( tileZoomLevel != m_tileZoomLevel ) {
        m_tileZoomLevel = tileZoomLevel;
        m_documents.clear();
    }

    const unsigned int maxTileX = ( 1 << tileZoomLevel ) * m_layer->levelZeroColumns();
    const unsigned int maxTileY = ( 1 << tileZoomLevel ) * m_layer->levelZeroRows();

    /** LOGIC FOR DOWNLOADING ALL THE TILES THAT ARE INSIDE THE SCREEN AT THE CURRENT ZOOM LEVEL **/

    // New tiles X and Y for moved screen coordinates
    // More info: http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#Subtiles
    // More info: http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#C.2FC.2B.2B
    // Sometimes the formula returns wrong huge values, x and y have to be between 0 and 2^ZoomLevel
    unsigned int minX = qMin<unsigned int>( maxTileX,
                              qMax<unsigned int>( lon2tileX( bbox.west(GeoDataCoordinates::Degree), maxTileX ),
                                    0 ) );

    unsigned int minY = qMin<unsigned int>( maxTileY,
                              qMax<unsigned int>( lat2tileY( bbox.north(GeoDataCoordinates::Degree), maxTileY ),
                                    0 ) );

    unsigned int maxX = qMax<unsigned int>( 0,
                              qMin<unsigned int>( lon2tileX( bbox.east(GeoDataCoordinates::Degree), maxTileX ),
                                    maxTileX ) );

    unsigned int maxY = qMax<unsigned int>( 0,
                              qMin<unsigned int>( lat2tileY( bbox.south(GeoDataCoordinates::Degree), maxTileY ),
                                    maxTileY ) );

    bool left  = minX < maxTileX;
    bool right = maxX > 0;
    bool up    = minY < maxTileY;
    bool down  = maxY > 0 ;

    // Download tiles and send them to VectorTileLayer
    // When changing zoom, download everything inside the screen
    if ( left && right && up && down )

                setViewport( tileZoomLevel, minX, minY, maxX, maxY );

    // When only moving screen, just download the new tiles
    else if ( left || right || up || down ){

        if ( left )
            setViewport( tileZoomLevel, minX, maxTileY, maxTileX, 0 );
        if ( right )
            setViewport( tileZoomLevel, 0, maxTileY, maxX, 0 );
        if ( up )
            setViewport( tileZoomLevel, maxTileX, minY, 0, maxTileY );
        if ( down )
            setViewport( tileZoomLevel, maxTileX, 0, 0, maxY );

        // During testing discovered that this code above does not request the "corner" tiles

    }

    removeTilesOutOfView(bbox);
}

void VectorTileModel::removeTilesOutOfView(const GeoDataLatLonBox &boundingBox)
{
    GeoDataLatLonBox const extendedViewport = boundingBox.scaled(3.0, 3.0);
    for (auto iter = m_documents.begin(); iter != m_documents.end();) {
        bool const isOutOfView = !extendedViewport.contains(iter.value()->m_boundingBox);
        if (isOutOfView) {
            iter = m_documents.erase(iter);
        }
        else {
            ++iter;
        }
    }
}

QString VectorTileModel::name() const
{
    return m_layer->name();
}

void VectorTileModel::removeTile(GeoDataDocument *document)
{
    emit tileRemoved(document);
}

int VectorTileModel::tileZoomLevel() const
{
    return m_tileZoomLevel;
}

int VectorTileModel::cachedDocuments() const
{
    return m_documents.size();
}

void VectorTileModel::updateTile( const TileId &id, GeoDataDocument *document )
{
    m_pendingDocuments.removeAll(id);
    if (!document) {
        return;
    }

    if ( m_tileZoomLevel != id.zoomLevel() ) {
        delete document;
        return;
    }

    document->setName(QString("%1/%2/%3").arg(id.zoomLevel()).arg(id.x()).arg(id.y()));
    m_garbageQueue << document;
    if (m_documents.contains(id)) {
        auto const document = m_documents.take(id);
        m_treeModel->removeFeature(document->m_document);
    }

    GeoDataLatLonBox const boundingBox = id.toLatLonBox(m_layer);
    m_documents[id] = QSharedPointer<CacheDocument>(new CacheDocument(document, this, boundingBox));
    emit tileAdded(document);
}

void VectorTileModel::clear()
{
    m_documents.clear();
}

void VectorTileModel::setViewport( int tileZoomLevel,
                                   unsigned int minTileX, unsigned int minTileY, unsigned int maxTileX, unsigned int maxTileY )
{
    // Download all the tiles inside the given indexes
    for ( unsigned int x = minTileX; x <= maxTileX; ++x ) {
        for ( unsigned int y = minTileY; y <= maxTileY; ++y ) {
           const TileId tileId = TileId( 0, tileZoomLevel, x, y );
           if ( !m_documents.contains( tileId ) && !m_pendingDocuments.contains( tileId ) ) {
               m_pendingDocuments << tileId;
               TileRunner *job = new TileRunner( m_loader, m_layer, tileId );
               connect( job, SIGNAL(documentLoaded(TileId,GeoDataDocument*)), this, SLOT(updateTile(TileId,GeoDataDocument*)) );
               m_threadPool->start( job );
           }
        }
    }
}

void VectorTileModel::cleanupTile(GeoDataObject *object)
{
    if (object->nodeType() == GeoDataTypes::GeoDataDocumentType) {
        GeoDataDocument* document = static_cast<GeoDataDocument*>(object);
        if (m_garbageQueue.contains(document)) {
            m_garbageQueue.removeAll(document);
            delete document;
        }
    }
}

unsigned int VectorTileModel::lon2tileX( qreal lon, unsigned int maxTileX )
{
    return (unsigned int)floor((lon + 180.0) / 360.0 * maxTileX);
}

unsigned int VectorTileModel::lat2tileY( qreal lat, unsigned int maxTileY )
{
    return (unsigned int)floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * maxTileY);
}

#include "moc_VectorTileModel.cpp"
