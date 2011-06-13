/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) <year>  <name of author>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "AltitudeModel.h"
#include "TileLoader.h"
#include "MarbleDebug.h"
#include "MapThemeManager.h"
#include <GeoSceneHead.h>
#include <GeoSceneMap.h>
#include <GeoSceneDocument.h>
#include <GeoSceneTexture.h>
#include "TextureTile.h"
#include <QLabel>
#include "TileLoaderHelper.h"
#include "MarbleModel.h"
#include <qmath.h>

namespace Marble {

AltitudeModel::AltitudeModel(const MapThemeManager*const mapThemeManager, HttpDownloadManager*const downloadManager, MarbleModel*const model)
    : QObject(0), m_model(model)
{
    m_cache.setMaxCost(10); //keep 10 tiles in memory (~17MB)
    m_mapThemeManager = mapThemeManager;
    //connect( mapThemeManager, SIGNAL( themesChanged() ),
    //         this, SLOT( updateTextureLayers() ) );
//     setTextureLayerSettings( textureLayerSettings );
    m_tileLoader = new TileLoader( downloadManager, mapThemeManager );
    updateTextureLayers();
    connect( m_tileLoader, SIGNAL( tileCompleted( TileId, QImage ) ),
             SLOT( tileCompleted( TileId, QImage ) ) );
}

//copied from TileLoader
void AltitudeModel::updateTextureLayers()
{
    m_textureLayer = 0;

    const GeoSceneDocument *srtmTheme = MapThemeManager::loadMapTheme( "earth/srtm2/srtm2.dgml" );
    Q_ASSERT( srtmTheme );

    const GeoSceneHead *head = srtmTheme->head();
    Q_ASSERT( head );

    const GeoSceneMap *map = srtmTheme->map();
    Q_ASSERT( map );

    const GeoSceneLayer *sceneLayer = map->layer( head->theme() );
    Q_ASSERT( sceneLayer );

    m_textureLayer = dynamic_cast<GeoSceneTexture*>( sceneLayer->datasets().first() );
    Q_ASSERT( m_textureLayer );
}

qreal AltitudeModel::height( qreal lat, qreal lon )
{
    const int tileZoomLevel = m_tileLoader->maximumTileLevel( *m_textureLayer );
    Q_ASSERT( tileZoomLevel == 9 );

    const int width = m_textureLayer->tileSize().width();
    const int height = m_textureLayer->tileSize().height();

    const int numTilesX = TileLoaderHelper::levelToColumn( m_textureLayer->levelZeroColumns(), tileZoomLevel );
    const int numTilesY = TileLoaderHelper::levelToRow( m_textureLayer->levelZeroRows(), tileZoomLevel );
    Q_ASSERT( numTilesX > 0 );
    Q_ASSERT( numTilesY > 0 );

    qreal textureX = 180 + lon;
    textureX *= numTilesX * width / 360;

    qreal textureY = 90 - lat;
    textureY *= numTilesY * height / 180;

    qreal ret = 0;

    for ( int i = 0; i < 4; ++i ) {
        const int x = static_cast<int>( textureX + ( i % 2 ) );
        const int y = static_cast<int>( textureY + ( i / 2 ) );

        //qDebug() << "x" << x << ( x / width );
        //qDebug() << "y" << y << ( y / height );

        const TileId id( "earth/srtm2", tileZoomLevel, ( x % ( numTilesX * width ) ) / width, ( y % ( numTilesY * height ) ) / height );
        //qDebug() << "tile" << ( x % ( numTilesX * width ) ) / width << ( y % ( numTilesY * height ) ) / height;

        const QImage *image = m_cache[id];
        if ( image == 0 ) {
            image = new QImage( m_tileLoader->loadTile( id, DownloadBrowse ) );
            m_cache.insert( id, image );
        }
        Q_ASSERT( image );
        Q_ASSERT( !image->isNull() );
        Q_ASSERT( width == image->width() );
        Q_ASSERT( height == image->height() );

        const qreal dx = ( textureX > (qreal)x ) ? textureX - (qreal)x : (qreal)x - textureX;
        const qreal dy = ( textureY > (qreal)y ) ? textureY - (qreal)y : (qreal)y - textureY;
        //qDebug() << "dx" << dx << "dy" << dy;
        Q_ASSERT( 0 <= dx && dx <= 1 );
        Q_ASSERT( 0 <= dy && dy <= 1 );
        unsigned int pixel;
        pixel = image->pixel( x % width, y % height );
        //qDebug() << "RGB" << qRed(pixel) << qGreen(pixel) << qBlue(pixel);
        pixel -= 0xFF000000; //fully opaque
        //qDebug() << "got at x" << x % width << "y" << y % height << "a height of" << pixel;

        ret += (qreal)pixel * (1-dx) * (1-dy);
    }

    //qDebug() << lat << lon << "altitude" << ret;
    return ret;
}

QList<qreal> AltitudeModel::heightProfile( qreal fromLat, qreal fromLon, qreal toLat, qreal toLon )
{
    const int tileZoomLevel = m_tileLoader->maximumTileLevel( *m_textureLayer );
    const int width = m_textureLayer->tileSize().width();
    const int numTilesX = TileLoaderHelper::levelToColumn( m_textureLayer->levelZeroColumns(), tileZoomLevel );

    qreal distPerPixel = (qreal)360 / ( width * numTilesX );
    //qDebug() << "heightProfile" << fromLat << fromLon << toLat << toLon << "distPerPixel" << distPerPixel;

    qreal lat = fromLat;
    qreal lon = fromLon;
    char dirLat = fromLat < toLat ? 1 : -1;
    char dirLon = fromLon < toLon ? 1 : -1;
    qreal k = ( fromLon - toLon ) / ( fromLat - toLat );
    //qDebug() << "dirLat" << QString::number(dirLat) << "dirLon" << QString::number(dirLon) << "k" << k;
    QList<qreal> ret;
    while ( lat*dirLat <= toLat*dirLat && lon*dirLon <= toLon*dirLon ) {
        ret << height( lat, lon );
        if ( k < 0.5 ) {
            lat += distPerPixel * dirLat;
            lon += distPerPixel * k;
        } else {
            lat += distPerPixel * k;
            lon += distPerPixel * dirLon;
        }
    }
    //qDebug() << ret;
    return ret;
}

void AltitudeModel::tileCompleted( const TileId & tileId, const QImage &image )
{
    m_cache.insert( tileId, new QImage( image ) );
    emit loadCompleted();
}

}



#include "AltitudeModel.moc"
