/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "VectorTileModel.h"

#include <QtCore/QRunnable>

#include "GeoDataDocument.h"
#include "GeoDataLatLonBox.h"
#include "MarbleGlobal.h"
#include "MarbleDebug.h"
#include "MathHelper.h"
#include "StackedTileLoader.h"
#include "StackedTile.h"
#include "TileId.h"

using namespace Marble;

VectorTileModel::VectorTileModel( StackedTileLoader *tileLoader )
    : m_tileLoader( tileLoader )
    , m_threadPool()
{
}

void VectorTileModel::setViewport( const GeoDataLatLonBox &bbox, int tileZoomLevel )
{
    const unsigned int maxTileX = m_tileLoader->tileColumnCount( tileZoomLevel );
    const unsigned int maxTileY = m_tileLoader->tileRowCount( tileZoomLevel );

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

void VectorTileModel::setViewport( int tileZoomLevel,
                                   unsigned int minTileX, unsigned int minTileY, unsigned int maxTileX, unsigned int maxTileY )
{
    // Reset backend
    m_tileLoader->resetTilehash();

    // Create render thread
    RenderJob *const job = new RenderJob( m_tileLoader, tileZoomLevel,
                                          minTileX, minTileY, maxTileX, maxTileY);

    // Connect the parser thread to the VectorTileMapper for recieving tiles
    connect( job, SIGNAL(tileCompleted(TileId,GeoDataDocument*,QString)),
             this, SIGNAL(tileCompleted(TileId,GeoDataDocument*,QString)) );

    // Start thread
    m_threadPool.start( job );

    // Aparently there is not needed to wait for it to finish but
    // waiting prevents having crashes with TileLoader's cache
    // when cleaning tile hash (duplicated node errors and double free errors)
    m_threadPool.waitForDone();

    m_tileLoader->cleanupTilehash();
}

unsigned int VectorTileModel::lon2tileX( qreal lon, unsigned int maxTileX )
{
    return (unsigned int)floor((lon + 180.0) / 360.0 * maxTileX);
}

unsigned int VectorTileModel::lat2tileY( qreal lat, unsigned int maxTileY )
{
    return (unsigned int)floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * maxTileY);
}

VectorTileModel::RenderJob::RenderJob( StackedTileLoader *tileLoader, int tileLevel,
                                        unsigned int minTileX, unsigned int minTileY, unsigned int maxTileX, unsigned int maxTileY )
    : m_tileLoader( tileLoader ),
      m_tileLevel( tileLevel ),
      m_minTileX( minTileX ),
      m_minTileY( minTileY ),
      m_maxTileX( maxTileX ),
      m_maxTileY( maxTileY )

{
}

void VectorTileModel::RenderJob::run()
{
    // Download all the tiles inside the given indexes
    for (unsigned int x = m_minTileX; x <= m_maxTileX; x++)
        for (unsigned int y = m_minTileY; y <= m_maxTileY; y++)
        {
           const TileId tileId = TileId( 0, m_tileLevel, x, y );
           const StackedTile * tile = m_tileLoader->loadTile( tileId );

           // When tile has vectorData send it to the VectorTileLayer for it to insert
           // it in the treeModel
           if ( tile->resultVectorData()->size() > 0 )
                emit tileCompleted( tileId, tile->resultVectorData(), "JS" );
        }
}

#include "VectorTileModel.moc"
