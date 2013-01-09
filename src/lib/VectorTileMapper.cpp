/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "VectorTileMapper.h"

#include <QtCore/QRunnable>

#include "MarbleGlobal.h"
#include "MarbleDebug.h"
#include "GeoPainter.h"
#include "GeoDataPolygon.h"
#include "Quaternion.h"
#include "ScanlineTextureMapperContext.h"
#include "StackedTileLoader.h"
#include "StackedTile.h"
#include "TextureColorizer.h"

#include "ViewportParams.h"
#include "MathHelper.h"


using namespace Marble;

VectorTileMapper::~VectorTileMapper()
{
}

VectorTileMapper::VectorTileMapper( StackedTileLoader *tileLoader )
    : TextureMapperInterface()
    , m_tileLoader( tileLoader )
    , m_repaintNeeded( true )
    , m_radius( 0 )
    , m_threadPool()
{
    m_minTileX = 0;
    m_minTileY = 0;
    m_maxTileX = 0;
    m_maxTileY = 0;
}

void VectorTileMapper::mapTexture( GeoPainter *painter,
                                   const ViewportParams *viewport,
                                   const QRect &dirtyRect,
                                   TextureColorizer *texColorizer )
{
    Q_UNUSED( texColorizer );

    /** LOGIC FOR DOWNLOADING ALL THE TILES THAT ARE INSIDE THE SCREEN AT THE CURRENT ZOOM LEVEL **/

    // New tiles X and Y for moved screen coordinates
    // More info: http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#Subtiles
    // More info: http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#C.2FC.2B.2B
    // Sometimes the formula returns wrong huge values, x and y have to be between 0 and 2^ZoomLevel
    unsigned int minX = qMin<unsigned int>( 2 << tileZoomLevel(),
                              qMax<unsigned int>( lon2tilex( viewport->viewLatLonAltBox().west(GeoDataCoordinates::Degree), tileZoomLevel() ),
                                    0 ) );

    unsigned int minY = qMin<unsigned int>( 2 << tileZoomLevel(),
                              qMax<unsigned int>( lat2tiley( viewport->viewLatLonAltBox().north(GeoDataCoordinates::Degree), tileZoomLevel() ),
                                    0 ) );

    unsigned int maxX = qMax<unsigned int>( 0,
                              qMin<unsigned int>( lon2tilex( viewport->viewLatLonAltBox().east(GeoDataCoordinates::Degree), tileZoomLevel() ),
                                    2 << tileZoomLevel() ) );

    unsigned int maxY = qMax<unsigned int>( 0,
                              qMin<unsigned int>( lat2tiley( viewport->viewLatLonAltBox().south(GeoDataCoordinates::Degree), tileZoomLevel() ),
                                    2 << tileZoomLevel() ) );

    bool left  = minX < m_minTileX;
    bool right = maxX > m_maxTileX;
    bool up    = minY < m_minTileY;
    bool down  = maxY > m_maxTileY ;

    // Download tiles and send them to VectorTileLayer
    // When changing zoom, download everything inside the screen
    if ( left && right && up && down )

                mapTexture( viewport, painter->mapQuality(), minX, minY, maxX, maxY );

    // When only moving screen, just download the new tiles
    else if ( left || right || up || down ){

        if ( left )
            mapTexture( viewport, painter->mapQuality(), minX, m_minTileY, m_minTileX, m_maxTileY );
        if ( right )
            mapTexture( viewport, painter->mapQuality(), m_maxTileX, m_minTileY, maxX, m_maxTileY );
        if ( up )
            mapTexture( viewport, painter->mapQuality(), m_minTileX, minY, m_maxTileX, m_minTileY );
        if ( down )
            mapTexture( viewport, painter->mapQuality(), m_minTileX, m_maxTileY, m_maxTileX, maxY );

        // During testing discovered that this code above does not request the "corner" tiles

    }

    // Update tiles X and Y for screen coordinates
    m_minTileX = minX;
    m_minTileY = minY;
    m_maxTileX = maxX;
    m_maxTileY = maxY;
}

void VectorTileMapper::setRepaintNeeded()
{
    m_repaintNeeded = true;
}

void VectorTileMapper::initTileRangeCoords(){

    // Set tile X and Y to the biggest possible values, but inverted so the
    // left/up/right/down variables can be calculated
    m_minTileX = 2 << tileZoomLevel();
    m_minTileY = 2 << tileZoomLevel();
    m_maxTileX = 0;
    m_maxTileY = 0;
}

void VectorTileMapper::mapTexture( const ViewportParams *viewport, MapQuality mapQuality,
                                   unsigned int minTileX, unsigned int minTileY, unsigned int maxTileX, unsigned int maxTileY )
{
    Q_UNUSED( mapQuality);

    // Reset backend
    m_tileLoader->resetTilehash();

    // Create render thread
    RenderJob *const job = new RenderJob( m_tileLoader, tileZoomLevel(), viewport,
                                          minTileX, minTileY, maxTileX, maxTileY);

    // Connect the parser thread to the VectorTileMapper for recieving tiles
    connect( job, SIGNAL( tileCompleted( TileId, GeoDataDocument*, QString ) ),
             this, SLOT( updateTile( TileId, GeoDataDocument*, QString ) ) );

    // Start thread
    m_threadPool.start( job );

    // Aparently there is not needed to wait for it to finish but
    // waiting prevents having crashes with TileLoader's cache
    // when cleaning tile hash (duplicated node errors and double free errors)
    m_threadPool.waitForDone();

    m_tileLoader->cleanupTilehash();
}

void VectorTileMapper::updateTile(TileId const & tileId, GeoDataDocument * document, QString const &format )
{
    // We received a vector tile, send it to the VectorTileLayer
    emit tileCompleted( tileId, document, format );
}

unsigned int VectorTileMapper::lon2tilex(double lon, int z)
{
    return (unsigned int)(floor((lon + 180.0) / 360.0 * (2 << z)));
}

unsigned int VectorTileMapper::lat2tiley(double lat, int z)
{
    return (unsigned int)(floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * (2 << z)));
}

VectorTileMapper::RenderJob::RenderJob( StackedTileLoader *tileLoader, int tileLevel, const ViewportParams *viewport,
                                        unsigned int minTileX, unsigned int minTileY, unsigned int maxTileX, unsigned int maxTileY )
    : m_tileLoader( tileLoader ),
      m_tileLevel( tileLevel ),
      m_viewport( viewport ),
      m_minTileX( minTileX ),
      m_minTileY( minTileY ),
      m_maxTileX( maxTileX ),
      m_maxTileY( maxTileY )

{
}

void VectorTileMapper::RenderJob::run()
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

#include "VectorTileMapper.moc"
