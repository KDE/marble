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
    m_mapThemeManager = mapThemeManager;
    //connect( mapThemeManager, SIGNAL( themesChanged() ),
    //         this, SLOT( updateTextureLayers() ) );
//     setTextureLayerSettings( textureLayerSettings );
    m_tileLoader = new TileLoader( downloadManager, mapThemeManager );
    updateTextureLayers();
//     connect( d->m_tileLoader, SIGNAL( tileCompleted( TileId, TileId )),
//              SLOT( updateTile( TileId, TileId )));
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

/*
    return textureLayer;
    
    QHash<uint, GeoSceneLayer const *> sceneLayers;

    QList<GeoSceneDocument const *> const & mapThemes = m_mapThemeManager->mapThemes();
    QList<GeoSceneDocument const *>::const_iterator pos = mapThemes.constBegin();
    QList<GeoSceneDocument const *>::const_iterator const end = mapThemes.constEnd();
    for (; pos != end; ++pos ) {
        GeoSceneHead const * head = (*pos)->head();
        Q_ASSERT( head );
        const QString mapThemeId = head->target() + '/' + head->theme();
        mDebug() << "AltitudeModel::updateTextureLayers" << mapThemeId;

        GeoSceneMap const * map = (*pos)->map();
        Q_ASSERT( map );
        GeoSceneLayer const * sceneLayer = map->layer( head->theme() );
        if ( !sceneLayer ) {
            mDebug() << "ignoring, has no GeoSceneLayer for" << head->theme();
            continue;
        }

        uint const mapThemeIdHash = qHash( mapThemeId );
        if ( sceneLayers.contains( mapThemeIdHash ) ) {
            mDebug() << "AltitudeModel::updateTextureLayers:"
                     << mapThemeIdHash << mapThemeId
                     << "already exists";
            continue;
        }

        sceneLayers.insert( mapThemeIdHash, sceneLayer );

        // find srtm2 texture layer
        QVector<GeoSceneAbstractDataset *> layers = sceneLayer->datasets();
        QVector<GeoSceneAbstractDataset *>::const_iterator pos = layers.constBegin();
        QVector<GeoSceneAbstractDataset *>::const_iterator const end = layers.constEnd();
        for (; pos != end; ++pos ) {
            GeoSceneTexture const * const textureLayer = dynamic_cast<GeoSceneTexture *>( *pos );
            if ( !textureLayer ) {
                mDebug() << "ignoring dataset, is not a texture layer";
                continue;
            }
            if ( textureLayer->sourceDir() != "earth/srtm2" ) {
                mDebug() << "ignoring dataset, not srtm2 layer";
                continue;
            }
            Q_ASSERT( !m_textureLayer ); //there should only be one
            m_textureLayer = textureLayer;
            mDebug() << "AltitudeModel::updateTextureLayers" << "using texture layer:"
                     << qHash( textureLayer->sourceDir() ) << textureLayer->sourceDir();
        }
    }
*/
    Q_ASSERT( m_textureLayer );
}

/*
1° = 1200px
360° = 432360px

1200*360 / 640 = 675


max zoom: 9 640 * 675 = 432000
          8  320 * 675 = 216000
          7  160 * 675 = 108000
          6  80 * 675
          5  40 * 675
          4  20 * 675
          3  10 * 675 = 6750
          2  5 * 675 = 3375
          1  2.5 * 675 = 1687.5
          0  1.25 * 675 = 843.75


          9 1024 * y = 432000
          8  512
          7  256
          6  128
          5   64
          4   32
          3   16
          2    8
          1    4
          0    2 * x = y*2



*/


qreal AltitudeModel::height( qreal lat, qreal lon )
{
    //QImage testTile = m_tileLoader->loadTile( TileId("earth/srtm2", 9, 548, 119), DownloadBrowse );



    
    const int tileZoomLevel = m_tileLoader->maximumTileLevel( *m_textureLayer );
    Q_ASSERT( tileZoomLevel == 9 );

    const int width = m_textureLayer->tileSize().width();
    const int height = m_textureLayer->tileSize().height();

    const int numTilesX = TileLoaderHelper::levelToColumn( m_textureLayer->levelZeroColumns(), tileZoomLevel );
    const int numTilesY = TileLoaderHelper::levelToRow( m_textureLayer->levelZeroRows(), tileZoomLevel );
    Q_ASSERT( numTilesX > 0 );
    Q_ASSERT( numTilesY > 0 );
//     qDebug() << "numTilesX" << numTilesX;

    //const int radius = m_model->planetRadius();

//---------> vom tileloader
    //imageL??Px: position in px of lon
    qreal textureX = 180 + lon;
//     qDebug() << "textureX(/1200)" << textureX << "*1200" << textureX * numTilesX * width;
    textureX *= numTilesX * width / 360;

    qreal textureY = 90 - lat;
//     qDebug() << "textureY(/1200)" << textureY << "*1200" << textureY * numTilesY * height;
    textureY *= numTilesY * height / 180;
//<------------
//Q_ASSERT(false);
    /*
    const qreal textureX = ( 0.5 + 0.5 * lon / M_PI ) * numTilesX * width;
    const qreal textureY = ( 0.5 - lat / M_PI ) * numTilesY * height;
    qDebug() << "textureX" << textureX;
    qDebug() << "textureY" << textureY;
    */


    const int x = static_cast<int>( textureX );
    const int y = static_cast<int>( textureY );

//     qDebug() << "x" << x << ( x / width );
//     qDebug() << "y" << y << ( y / height );

    qDebug() << "x2" << x << ( x % ( numTilesX * width ) ) / width;
    qDebug() << "y2" << y << ( y % ( numTilesY * height ) ) / height;
    //const TileId id( "earth/srtm2", tileZoomLevel, x / width, y / height );
    const TileId id( "earth/srtm2", tileZoomLevel, ( x % ( numTilesX * width ) ) / width, ( y % ( numTilesY * height ) ) / height );
    //const TileId id("earth/srtm2", 9, 548, 119);

    //const QImage *image = m_cache[id];
    //if ( image == 0 )
    //{
    //    QSharedPointer<TextureTile> tile = m_tileLoader->loadTile( id, id, DownloadBrowse );
    //    image = new QImage( *tile->image() );
    //    m_cache.insert( id, image );
    //}
    QImage image = m_tileLoader->loadTile( id, DownloadBrowse );

    Q_ASSERT( !image.isNull() );
    Q_ASSERT( width == image.width() );
    Q_ASSERT( height == image.height() );

    //const qreal dx = ( textureX > (qreal)x ) ? textureX - (qreal)x : (qreal)x - textureX;
    //const qreal dy = ( textureY > (qreal)y ) ? textureY - (qreal)y : (qreal)y - textureY;
    //Q_ASSERT( 0 <= dx && dx <= 1 );
    //Q_ASSERT( 0 <= dy && dy <= 1 );
    unsigned int pixel;
    pixel = image.pixel( x % width, y % height );
    qDebug() << "RGB" << qRed(pixel) << qGreen(pixel) << qBlue(pixel);
    pixel -= 0xFF000000; //fully opaque
    qDebug() << "got at x" << x % width << "y" << y % height << "a height of" << pixel;
    //color += image.pixel( x % width, y % height ) * (1- dx) * (1-dy);

    //qDebug() << color;
    //Q_ASSERT(0);

    return pixel;
    //return color * radius / 6378137.0 / 1000;

/*

    const qreal xNormalizedCenter = 0.5 + 0.5 * centerLon / M_PI;
    const int minTileX = qFloor( numTilesX * ( xNormalizedCenter - imageWidth/( 8.0 * radius ) ) );
    const int maxTileX = numTilesX * ( xNormalizedCenter + imageWidth/( 8.0 * radius ) );

    const qreal yNormalizedCenter = 0.5 - 0.5 * asinh( tan( centerLat ) ) / M_PI;
    const int minTileY = qMax( qreal( numTilesY * ( yNormalizedCenter - imageHeight/( 8.0 * radius ) ) ),
                               qreal( 0.0 ) );
    const int maxTileY = qMin( qreal( numTilesY * ( yNormalizedCenter + imageHeight/( 8.0 * radius ) ) ),
                               qreal( numTilesY - 1.0 ) );

    if ( texColorizer || m_oldRadius != radius ) {
        m_cache->clear();

        QPainter painter( canvasImage.data() );
        painter.setRenderHint( QPainter::SmoothPixmapTransform, highQuality );

        for ( int tileY = minTileY; tileY <= maxTileY; ++tileY ) {
            for ( int tileX = minTileX; tileX <= maxTileX; ++tileX ) {
                const qreal xLeft   = ( 4.0 * radius ) * ( ( tileX     ) / (qreal)numTilesX - xNormalizedCenter ) + ( imageWidth / 2.0 );
                const qreal xRight  = ( 4.0 * radius ) * ( ( tileX + 1 ) / (qreal)numTilesX - xNormalizedCenter ) + ( imageWidth / 2.0 );
                const qreal yTop    = ( 4.0 * radius ) * ( ( tileY     ) / (qreal)numTilesY - yNormalizedCenter ) + ( imageHeight / 2.0 );
                const qreal yBottom = ( 4.0 * radius ) * ( ( tileY + 1 ) / (qreal)numTilesY - yNormalizedCenter ) + ( imageHeight / 2.0 );

                const QRectF rect = QRectF( QPointF( xLeft, yTop ), QPointF( xRight, yBottom ) );
                const TileId stackedId = TileId( 0, tileZoomLevel(), ( ( tileX % numTilesX ) + numTilesX ) % numTilesX, tileY );
                StackedTile *const tile = m_tileLoader->loadTile( stackedId );
                tile->setUsed( true );

                painter.drawImage( rect, *tile->resultTile() );


            }
        }
    }
*/
    TileId tileId("earth/srtm2", 9, 548, 119);
    QImage tile = m_tileLoader->loadTile( tileId, DownloadBrowse );

    Q_ASSERT(!tile.isNull());
    qDebug() << tile.size();

    tile.save("/home/niko/srtm2.jpg");

    QLabel* l = new QLabel();
    l->setPixmap(QPixmap::fromImage(tile));
    l->show();

    return 0;
}

}



#include "AltitudeModel.moc"
