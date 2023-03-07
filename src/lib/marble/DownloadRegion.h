// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DOWNLOADREGION_H
#define MARBLE_DOWNLOADREGION_H

#include <QObject>
#include <QVector>

#include <MarbleGlobal.h>

#include "marble_export.h"

namespace Marble
{
class DownloadRegionPrivate;
class GeoDataLatLonAltBox;
class GeoDataLineString;
class TileCoordsPyramid;
class MarbleModel;
class TileLayer;
class TextureLayer;

class MARBLE_EXPORT DownloadRegion : public QObject
{
    Q_OBJECT

 public:
    explicit DownloadRegion( QObject* parent=nullptr );

    void setMarbleModel( MarbleModel *model );

    ~DownloadRegion() override;

    void setTileLevelRange( int const minimumTileLevel, int const maximumTileLevel );

    QVector<TileCoordsPyramid> region( const TileLayer *tileLayer, const GeoDataLatLonAltBox &region ) const;

    void setVisibleTileLevel( int const tileLevel );

    /**
      * @brief calculates the region to be downloaded around a path
      */
    QVector<TileCoordsPyramid> fromPath( const TileLayer *tileLayer, qreal offset, const GeoDataLineString &path ) const;

    QVector<int> validTileLevels( const TileType tileType ) const;

  private:
    DownloadRegionPrivate* const d;
};

}

#endif
