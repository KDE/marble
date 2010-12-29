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

#ifndef MARBLE_MERGEDLAYERDECORATOR_H
#define MARBLE_MERGEDLAYERDECORATOR_H

#include <QtGui/QImage>
#include <QtCore/QObject>

#include "TileId.h"
#include "global.h"

class QString;
class QUrl;

namespace Marble
{
class GeoSceneDocument;
class GeoSceneTexture;
class SunLocator;
class StackedTile;
class TileLoader;

class MergedLayerDecorator : public QObject
{
    Q_OBJECT

 public:
    MergedLayerDecorator( TileLoader * const tileLoader, SunLocator* sunLocator );
    virtual ~MergedLayerDecorator();

    // The Parameter themeId is only used for displaying the TileId,
    // which is a debugging feature, therefore at this point QString remains.
    void paint( const QString& themeId );

    void setShowTileId(bool show);
    bool showTileId() const;

    void setTile(QImage* tile);
    void setInfo( TileId const &id );

 Q_SIGNALS:
    void repaintMap();

 private:
    QImage loadDataset();
    int maxDivisor( int maximum, int fullLength );

    void initCityLights();

    void paintSunShading();
    void paintTileId(const QString& themeId);

 protected:
    Q_DISABLE_COPY( MergedLayerDecorator )
    TileLoader * const m_tileLoader;
    QImage* m_tile;
    TileId m_id;
    SunLocator* m_sunLocator;
    bool m_showTileId;
    GeoSceneDocument *m_cityLightsTheme;
    GeoSceneTexture *m_cityLightsTextureLayer;
};

}

#endif
