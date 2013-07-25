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
#include "GeoSceneVectorTile.h"
#include "MarbleGlobal.h"
#include "MarbleDebug.h"
#include "MathHelper.h"
#include "TileId.h"
#include "TileLoader.h"

#include <qmath.h>
#include <QThreadPool>

using namespace Marble;

TileRunner::TileRunner( TileLoader *loader, const GeoSceneVectorTile *texture, const TileId &id ) :
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

VectorTileModel::CacheDocument::CacheDocument( GeoDataDocument *doc, GeoDataTreeModel *model ) :
    m_document( doc ),
    m_treeModel( model )
{
    // nothing to do
}

VectorTileModel::CacheDocument::~CacheDocument()
{
    Q_ASSERT( m_treeModel );
    m_treeModel->removeDocument( m_document );
    delete m_document;
}

VectorTileModel::VectorTileModel( TileLoader *loader, const GeoSceneVectorTile *layer, GeoDataTreeModel *treeModel, QThreadPool *threadPool ) :
    m_loader( loader ),
    m_layer( layer ),
    m_treeModel( treeModel ),
    m_threadPool( threadPool ),
    m_tileZoomLevel( -1 )
{
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
}

QString VectorTileModel::name() const
{
    return m_layer->name();
}

void VectorTileModel::updateTile( const TileId &id, GeoDataDocument *document )
{
    if ( m_tileZoomLevel != id.zoomLevel() ) {
        delete document;
        return;
    }

    m_treeModel->addDocument( document );
    m_documents.insert( id, new CacheDocument( document, m_treeModel ) );
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

           if ( !m_documents.contains( tileId ) ) {
               GeoDataDocument *const document = new GeoDataDocument;

               TileRunner *job = new TileRunner( m_loader, m_layer, tileId );
               connect( job, SIGNAL(documentLoaded(TileId,GeoDataDocument*)), this, SLOT(updateTile(TileId,GeoDataDocument*)) );
               m_threadPool->start( job );

               m_treeModel->addDocument( document );
               m_documents.insert( tileId, new CacheDocument( document, m_treeModel ) );
           }
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

#include "VectorTileModel.moc"
