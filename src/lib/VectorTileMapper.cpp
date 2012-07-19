/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2007      Torsten Rahn     <tackat@kde.org>
 Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
*/

#include "VectorTileMapper.h"

#include <cmath>

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
}

void VectorTileMapper::mapTexture( GeoPainter *painter,
                                   const ViewportParams *viewport,
                                   const QRect &dirtyRect,
                                   TextureColorizer *texColorizer )
{
    if ( m_canvasImage.size() != viewport->size() || m_radius != viewport->radius() ) {
        const QImage::Format optimalFormat = ScanlineTextureMapperContext::optimalCanvasImageFormat( viewport );

        if ( m_canvasImage.size() != viewport->size() || m_canvasImage.format() != optimalFormat ) {
            m_canvasImage = QImage( viewport->size(), optimalFormat );
        }

        if ( !viewport->mapCoversViewport() ) {
            m_canvasImage.fill( 0 );
        }

        //        m_radius = viewport->radius();
        m_repaintNeeded = true;
    }

    if ( m_repaintNeeded ) {

        mapTexture( viewport, painter->mapQuality() );

        m_repaintNeeded = false;
    }

    const int radius = (int)(1.05 * (qreal)(viewport->radius()));

    QRect rect( viewport->width() / 2 - radius, viewport->height() / 2 - radius,
                2 * radius, 2 * radius);
    rect = rect.intersect( dirtyRect );
    painter->drawImage( rect, m_canvasImage, rect );
}

void VectorTileMapper::setRepaintNeeded()
{
    m_repaintNeeded = true;
}

void VectorTileMapper::mapTexture( const ViewportParams *viewport, MapQuality mapQuality )
{
    Q_UNUSED( mapQuality);

    // Reset backend
    m_tileLoader->resetTilehash();

    RenderJob *const job = new RenderJob( m_tileLoader, tileZoomLevel(), viewport );

    // Connect the parser thread to the VectorTileMapper for recieving tiles
    connect( job, SIGNAL( tileCompleted( TileId, GeoDataDocument*, QString ) ),
             this, SLOT( updateTile( TileId, GeoDataDocument*, QString ) ) );

    // Start parsing
    m_threadPool.start( job );

    // FIXME ANDER COMMENTING THE LINE AT MARBLEMODEL WE DON'T NEED TO WAIT FOR FINISHING
    // m_threadPool.waitForDone();

    m_tileLoader->cleanupTilehash();
}

void VectorTileMapper::updateTile(TileId const & tileId, GeoDataDocument * document, QString const &format )
{
    // We recieved a vector tile, send it to the VectorTileLayer
    emit tileCompleted( tileId, document, format );
}

VectorTileMapper::RenderJob::RenderJob( StackedTileLoader *tileLoader, int tileLevel, const ViewportParams *viewport )
    : m_tileLoader( tileLoader ),
      m_tileLevel( tileLevel ),
      m_viewport( viewport )
{
}

void VectorTileMapper::RenderJob::run()
{
    /** FIXME ANDER TEST LOGIC FOR DOWNLOADING ALL THE TILES THAT ARE IN THE CURRENT ZOOM LEVEL AND INSIDE THE SCREEN **/

    int minTileX = lon2tilex( m_viewport->viewLatLonAltBox().west(GeoDataCoordinates::Degree), m_tileLevel );
    int minTileY = lat2tiley( m_viewport->viewLatLonAltBox().north(GeoDataCoordinates::Degree), m_tileLevel );
    int maxTileX = lon2tilex( m_viewport->viewLatLonAltBox().east(GeoDataCoordinates::Degree), m_tileLevel );
    int maxTileY = lat2tiley( m_viewport->viewLatLonAltBox().south(GeoDataCoordinates::Degree), m_tileLevel );

    for (int x = minTileX; x < maxTileX; x++)
        for (int y = minTileY; y < maxTileY; y++)
            if ( x >= 0 && y >= 0){
                const TileId tileId = TileId( 0, m_tileLevel, x, y );
                const StackedTile * tile = m_tileLoader->loadTile( tileId );

                if ( tile->resultVectorData()->size() > 0 )
                    emit tileCompleted( tileId, tile->resultVectorData(), "JS" );
            }
}

int VectorTileMapper::RenderJob::lon2tilex(double lon, int z)
{
    return (int)(floor((lon + 180.0) / 360.0 * pow(2.0, z)));
}

int VectorTileMapper::RenderJob::lat2tiley(double lat, int z)
{
    return (int)(floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z)));
}

#include "VectorTileMapper.moc"
