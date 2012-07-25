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
    if ( m_canvasImage.size() != viewport->size() || m_radius != viewport->radius() ) {
        const QImage::Format optimalFormat = ScanlineTextureMapperContext::optimalCanvasImageFormat( viewport );

        if ( m_canvasImage.size() != viewport->size() || m_canvasImage.format() != optimalFormat ) {
            m_canvasImage = QImage( viewport->size(), optimalFormat );
        }

        if ( !viewport->mapCoversViewport() ) {
            //m_canvasImage.fill( QColor(0, 0, 0, 255).rgba() );
            m_canvasImage.fill( 0 );
        }

        // m_radius = viewport->radius();
        m_repaintNeeded = true;
    }

    m_repaintNeeded = true;

    if ( m_repaintNeeded ) {

        /** FIXME ANDER TEST LOGIC FOR DOWNLOADING ALL THE TILES THAT ARE IN THE CURRENT ZOOM LEVEL AND INSIDE THE SCREEN **/

        int minX = lon2tilex( viewport->viewLatLonAltBox().west(GeoDataCoordinates::Degree), tileZoomLevel() )-1;
        int minY = lat2tiley( viewport->viewLatLonAltBox().north(GeoDataCoordinates::Degree), tileZoomLevel() )-1;
        int maxX = lon2tilex( viewport->viewLatLonAltBox().east(GeoDataCoordinates::Degree), tileZoomLevel() )+1;
        int maxY = lat2tiley( viewport->viewLatLonAltBox().south(GeoDataCoordinates::Degree), tileZoomLevel() )+1;

        bool left = minX < m_minTileX;
        bool right = maxX > m_maxTileX;
        bool up = minY < m_minTileY;
        bool down = maxY > m_maxTileY ;

        if ( left || right || up || down ){

            mDebug() << "-----------------------------------0" << left << up << right << down;

            int l = left? minX : right? m_maxTileX : m_minTileX;
            int u = up? minY : down? m_maxTileY : m_minTileY;
            int r = left? m_minTileX : right? maxX : m_maxTileX;
            int d = up? m_minTileY : down? maxY : m_maxTileY;

            mDebug() << "-----------------------------------1" << l << u << r << d;

            l = minX;
            r = maxX;
            u = minY;
            d = maxY;

            mDebug() << "-----------------------------------2" << l << u << r << d;

//            mapTexture( viewport, painter->mapQuality(),
//                        left? minX : right? m_maxTileX : m_minTileX,
//                        up? minY : down? m_maxTileY : m_minTileY,
//                        left? m_minTileX : right? maxX : m_maxTileX,
//                        up? m_minTileY : down? maxY : m_maxTileY );
            mapTexture( viewport, painter->mapQuality(), minX, minY, maxX, maxY );
        }

        m_minTileX = minX;
        m_minTileY = minY;
        m_maxTileX = maxX;
        m_maxTileY = maxY;

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

void VectorTileMapper::zoomChanged(){
    m_minTileX = pow( 2, tileZoomLevel() );
    m_minTileY = pow( 2, tileZoomLevel() );
    m_maxTileX = 0;
    m_maxTileY = 0;
}

void VectorTileMapper::mapTexture( const ViewportParams *viewport, MapQuality mapQuality,
                                   int minTileX, int minTileY, int maxTileX, int maxTileY )
{
    Q_UNUSED( mapQuality);

    // Reset backend
    m_tileLoader->resetTilehash();

    RenderJob *const job = new RenderJob( m_tileLoader, tileZoomLevel(), viewport,
                                          minTileX, minTileY, maxTileX, maxTileY);

    // Connect the parser thread to the VectorTileMapper for recieving tiles
    connect( job, SIGNAL( tileCompleted( TileId, GeoDataDocument*, QString ) ),
             this, SLOT( updateTile( TileId, GeoDataDocument*, QString ) ) );

    // Start parsing
    m_threadPool.start( job );

    // FIXME ANDER
    // m_threadPool.waitForDone();

    m_tileLoader->cleanupTilehash();

    mDebug() << "------------------------DESCARGAR" << minTileX << minTileY << maxTileX << maxTileY;
}

void VectorTileMapper::updateTile(TileId const & tileId, GeoDataDocument * document, QString const &format )
{
    // We recieved a vector tile, send it to the VectorTileLayer
    emit tileCompleted( tileId, document, format );
}

int VectorTileMapper::lon2tilex(double lon, int z)
{
    return (int)(floor((lon + 180.0) / 360.0 * pow(2.0, z)));
}

int VectorTileMapper::lat2tiley(double lat, int z)
{
    return (int)(floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z)));
}

VectorTileMapper::RenderJob::RenderJob( StackedTileLoader *tileLoader, int tileLevel, const ViewportParams *viewport,
                                        int minTileX, int minTileY, int maxTileX, int maxTileY )
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
    for (int x = m_minTileX; x < m_maxTileX; x++)
        for (int y = m_minTileY; y < m_maxTileY; y++)
            if ( x >= 0 && y >= 0){
                const TileId tileId = TileId( 0, m_tileLevel, x, y );
                const StackedTile * tile = m_tileLoader->loadTile( tileId );

                if ( tile->resultVectorData()->size() > 0 )
                    emit tileCompleted( tileId, tile->resultVectorData(), "JS" );
            }
}

#include "VectorTileMapper.moc"
