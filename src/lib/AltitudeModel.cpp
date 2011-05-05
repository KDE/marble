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

namespace Marble {

AltitudeModel::AltitudeModel(const MapThemeManager*const mapThemeManager, HttpDownloadManager*const downloadManager, MarbleModel*const model)
    : QObject(0)
{
    m_mapThemeManager = mapThemeManager;
    connect( mapThemeManager, SIGNAL( themesChanged() ),
             this, SLOT( updateTextureLayers() ) );
//     setTextureLayerSettings( textureLayerSettings );
    m_tileLoader = new TileLoader( downloadManager, mapThemeManager );
    updateTextureLayers();
//     connect( d->m_tileLoader, SIGNAL( tileCompleted( TileId, TileId )),
//              SLOT( updateTile( TileId, TileId )));
}


//copied from StackedTileLoader
void AltitudeModel::updateTextureLayers()
{
    QHash<uint, GeoSceneTexture const *> textureLayers;

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

        // find all texture layers
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
            textureLayers.insert( qHash( textureLayer->sourceDir() ), textureLayer );
            mDebug() << "AltitudeModel::updateTextureLayers" << "added texture layer:"
                     << qHash( textureLayer->sourceDir() ) << textureLayer->sourceDir();
        }
    }

    //TODO? m_tileLoader->setTextureLayers( textureLayers );
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
    TileId tileId("earth/srtm2", 12, 123, 456);
    QImage tile = m_tileLoader->loadTile( tileId, DownloadBrowse );

    Q_ASSERT(!tile.isNull());
    
    QLabel* l = new QLabel();
    l->setPixmap(QPixmap::fromImage(tile));
    l->show();
//     http://localhost/maps/earth/srtm/12/000456/000456_000123.jpg

    return 0;
}

}



#include "AltitudeModel.moc"
