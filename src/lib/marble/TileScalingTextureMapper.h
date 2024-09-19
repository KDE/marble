// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_TILESCALINGTEXTUREMAPPER_H
#define MARBLE_TILESCALINGTEXTUREMAPPER_H

#include "TextureMapperInterface.h"
#include <QObject>

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
    explicit TileScalingTextureMapper(StackedTileLoader *tileLoader, QObject *parent = nullptr);

    void mapTexture(GeoPainter *painter, const ViewportParams *viewport, int tileZoomLevel, const QRect &dirtyRect, TextureColorizer *texColorizer) override;

private Q_SLOTS:
    void removePixmap(const TileId &tileId);
    void clearPixmaps();

private:
    void mapTexture(GeoPainter *painter, const ViewportParams *viewport, int tileZoomLevel, TextureColorizer *texColorizer);

private:
    StackedTileLoader *const m_tileLoader;
    QCache<TileId, const QPixmap> m_cache;
    QImage m_canvasImage;
    int m_radius;
};

}

#endif
