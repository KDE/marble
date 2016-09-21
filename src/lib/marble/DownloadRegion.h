//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DOWNLOADREGION_H
#define MARBLE_DOWNLOADREGION_H

#include <QObject>
#include <QVector>

#include "marble_export.h"

namespace Marble
{
class DownloadRegionPrivate;
class GeoDataLatLonAltBox;
class GeoDataLineString;
class TileCoordsPyramid;
class MarbleModel;
class TextureLayer;

class MARBLE_EXPORT DownloadRegion : public QObject
{
    Q_OBJECT

 public:
    explicit DownloadRegion( QObject* parent=0 );

    void setMarbleModel( MarbleModel *model );

    ~DownloadRegion();

    void setTileLevelRange( int const minimumTileLevel, int const maximumTileLevel );

    QVector<TileCoordsPyramid> region( const TextureLayer *textureLayer, const GeoDataLatLonAltBox &region ) const;

    void setVisibleTileLevel( int const tileLevel );

    /**
      * @brief calculates the region to be downloaded around a path
      */
    QVector<TileCoordsPyramid> fromPath( const TextureLayer *textureLayer, qreal offset, const GeoDataLineString &path ) const;

  private:
    DownloadRegionPrivate* const d;
};

}

#endif
