//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn     <tackat@kde.org>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#include "VectorTileMapper.h"

#include <cmath>

#include <QtCore/QRunnable>

#include "MarbleGlobal.h"
#include "GeoPainter.h"
#include "GeoDataPolygon.h"
#include "GeoDataDocument.h"
#include "MarbleDebug.h"
#include "Quaternion.h"
#include "ScanlineTextureMapperContext.h"
#include "StackedTileLoader.h"
#include "StackedTile.h"
#include "TextureColorizer.h"
#include "ViewportParams.h"
#include "MathHelper.h"


using namespace Marble;

class VectorTileMapper::RenderJob : public QRunnable
{
public:
    RenderJob( StackedTileLoader *tileLoader, int tileLevel, const ViewportParams *viewport );

    virtual void run();

    int lon2tilex(double lon, int z);

    int lat2tiley(double lat, int z);

private:
    StackedTileLoader *const m_tileLoader;
    const int m_tileLevel;
    const ViewportParams *const m_viewport;
};

VectorTileMapper::RenderJob::RenderJob( StackedTileLoader *tileLoader, int tileLevel, const ViewportParams *viewport )
    : m_tileLoader( tileLoader ),
      m_tileLevel( tileLevel ),
      m_viewport( viewport )
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

        m_radius = viewport->radius();
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

    QRunnable *const job = new RenderJob( m_tileLoader, tileZoomLevel(), viewport );

    m_threadPool.start( job );

    m_threadPool.waitForDone();

    m_tileLoader->cleanupTilehash();
}

void VectorTileMapper::RenderJob::run()
{
    /** FIXME ANDER TEST LOGIC FOR DOWNLOADING ALL THE TILES THAT ARE IN THE CURRENT ZOOM LEVEL AND INSIDE THE SCREEN **/

    // More info: http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#Subtiles

    int minTileX = lon2tilex( m_viewport->viewLatLonAltBox().west(GeoDataCoordinates::Degree), m_tileLevel );
    int minTileY = lat2tiley( m_viewport->viewLatLonAltBox().north(GeoDataCoordinates::Degree), m_tileLevel );
    int maxTileX = lon2tilex( m_viewport->viewLatLonAltBox().east(GeoDataCoordinates::Degree), m_tileLevel );
    int maxTileY = lat2tiley( m_viewport->viewLatLonAltBox().south(GeoDataCoordinates::Degree), m_tileLevel );

    for (int x = minTileX; x < maxTileX; x++)
        for (int y = minTileY; y < maxTileY; y++)
            if ( x >= 0 && y >= 0)
            m_tileLoader->loadTile(TileId( 0, m_tileLevel, x, y ));
}

int VectorTileMapper::RenderJob::lon2tilex(double lon, int z)
{
    return (int)(floor((lon + 180.0) / 360.0 * pow(2.0, z)));
}

int VectorTileMapper::RenderJob::lat2tiley(double lat, int z)
{
    return (int)(floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z)));
}
