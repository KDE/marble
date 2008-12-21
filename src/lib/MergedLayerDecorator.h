// Copyright 2008 David Roberts <dvdr18@gmail.com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.

#ifndef MERGED_LAYER_DECORATOR_H
#define MERGED_LAYER_DECORATOR_H

#include <QtGui/QImage>
#include <QtCore/QObject>

#include "SunLocator.h"
#include "TileId.h"

class QString;
class QUrl;

namespace Marble
{
class GeoSceneDocument;
class GeoSceneTexture;

class MergedLayerDecorator : public QObject
{
    Q_OBJECT
	
 public:
    explicit MergedLayerDecorator(SunLocator* sunLocator);
    virtual ~MergedLayerDecorator();

    // The Parameter themeId is only used for displaying the TileId,
    // which is a debugging feature, therefore at this point QString remains.
    void paint( const QString& themeId, GeoSceneDocument *mapTheme = 0 );
    void paintTileId(const QString& themeId);
    
    void setShowClouds(bool show);
    bool showClouds() const;

    void setShowTileId(bool show);
    bool showTileId() const;
	
    void setTile(QImage* tile);
    void setInfo(int x, int y, int level, TileId const& id);
	
 Q_SIGNALS:
    void downloadTile(const QUrl& sourceUrl, const QString& destinationFileName,
                      const QString& id);
    void repaintMap();
	
 private:
    QImage loadDataset( GeoSceneTexture *textureLayer );
    int maxDivisor( int maximum, int fullLength );
    void paintSunShading();
    void paintClouds();
	
 protected:
    Q_DISABLE_COPY( MergedLayerDecorator )
    QImage* m_tile;
    int m_x;
    int m_y;
    int m_level;
    TileId m_id;
    SunLocator* m_sunLocator;
    bool m_cloudlayer;
    bool m_showTileId;
    GeoSceneDocument *m_cityLightsTheme;
    GeoSceneDocument *m_blueMarbleTheme;
    GeoSceneTexture *m_cityLightsTextureLayer;
    GeoSceneTexture *m_cloudsTextureLayer;
};

}

#endif
