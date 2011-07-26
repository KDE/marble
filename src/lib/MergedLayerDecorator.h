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

#include <QtCore/QMutex>
#include <QtCore/QSharedPointer>
#include <QtCore/QVector>
#include <QtGui/QImage>

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
class TextureTile;
class TileLoader;

class MergedLayerDecorator
{
 public:
    MergedLayerDecorator( TileLoader * const tileLoader, SunLocator* sunLocator );
    virtual ~MergedLayerDecorator();

    QImage merge( const TileId id, const QVector<QSharedPointer<TextureTile> > &tiles );

    void setThemeId( const QString &themeId );

    void setLevelZeroLayout( int levelZeroColumns, int levelZeroRows );

    void setShowTileId(bool show);

 private:
    QImage loadDataset( const TileId &id );
    static int maxDivisor( int maximum, int fullLength );

    void initCityLights();

    void paintCityLights( QImage *tileImage, const TileId &id );
    void paintSunShading( QImage *tileImage, const TileId &id ) const;
    void paintTileId( QImage *tileImage, const TileId &id ) const;

 protected:
    Q_DISABLE_COPY( MergedLayerDecorator )
    TileLoader * const m_tileLoader;
    SunLocator* m_sunLocator;
    QString m_themeId;
    int m_levelZeroColumns;
    int m_levelZeroRows;
    bool m_showTileId;
    GeoSceneDocument *m_cityLightsTheme;
    GeoSceneTexture *m_cityLightsTextureLayer;
    QMutex m_initCityLightsMutex;
};

}

#endif
