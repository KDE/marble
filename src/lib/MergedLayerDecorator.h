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

#include <QtCore/QSharedPointer>
#include <QtCore/QVector>

#include "MarbleGlobal.h"

class QImage;
class QString;

namespace Marble
{
class GeoSceneTiled;
class SunLocator;
class StackedTile;
class Tile;
class TileId;
class TileLoader;
class GeoDataDocument;

class MergedLayerDecorator
{
 public:
    MergedLayerDecorator( TileLoader * const tileLoader, const SunLocator* sunLocator );
    virtual ~MergedLayerDecorator();

    StackedTile *loadTile( const TileId &id, const QVector<const GeoSceneTiled *> &textureLayers ) const;

    StackedTile *createTile( const StackedTile &stackedTile, const TileId &tileId, const QImage &tileImage, GeoDataDocument * tileData ) const;

    void downloadStackedTile( const TileId &id, const QVector<GeoSceneTiled const *> &textureLayers, DownloadUsage usage );

    void reloadTile( const StackedTile &stackedTile );

    void setThemeId( const QString &themeId );

    void setLevelZeroLayout( int levelZeroColumns, int levelZeroRows );

    void setShowSunShading( bool show );
    bool showSunShading() const;

    void setShowCityLights( bool show );
    bool showCityLights() const;

    void setShowTileId(bool show);

 protected:
    Q_DISABLE_COPY( MergedLayerDecorator )

    class Private;
    Private *const d;
};

}

#endif
