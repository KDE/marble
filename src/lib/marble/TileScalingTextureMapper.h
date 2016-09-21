//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_TILESCALINGTEXTUREMAPPER_H
#define MARBLE_TILESCALINGTEXTUREMAPPER_H


#include <QObject>
#include "TextureMapperInterface.h"

#include "TileId.h"

#include <QCache>
#include <QImage>
#include <QPixmap>

namespace Marble
{

class StackedTileLoader;

class TileScalingTextureMapper : public QObject, public TextureMapperInterface
{
    Q_OBJECT

 public:
    explicit TileScalingTextureMapper( StackedTileLoader *tileLoader, QObject *parent = 0 );

    virtual void mapTexture( GeoPainter *painter,
                             const ViewportParams *viewport,
                             int tileZoomLevel,
                             const QRect &dirtyRect,
                             TextureColorizer *texColorizer );

 private Q_SLOTS:
    void removePixmap( const TileId &tileId );
    void clearPixmaps();

 private:
    void mapTexture( GeoPainter *painter,
                     const ViewportParams *viewport,
                     int tileZoomLevel,
                     TextureColorizer *texColorizer );

 private:
    StackedTileLoader *const m_tileLoader;
    QCache<TileId, const QPixmap> m_cache;
    QImage m_canvasImage;
    int    m_radius;
};

}

#endif
