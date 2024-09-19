// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Carlos Licea <carlos _licea@hotmail.com>
// SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>
// SPDX-FileCopyrightText: 2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_MERCATORSCANLINETEXTUREMAPPER_H
#define MARBLE_MERCATORSCANLINETEXTUREMAPPER_H

#include "TextureMapperInterface.h"

#include "MarbleGlobal.h"

#include <QImage>
#include <QThreadPool>

namespace Marble
{

class StackedTileLoader;

class MercatorScanlineTextureMapper : public TextureMapperInterface
{
public:
    explicit MercatorScanlineTextureMapper(StackedTileLoader *tileLoader);

    void mapTexture(GeoPainter *painter, const ViewportParams *viewport, int tileZoomLevel, const QRect &dirtyRect, TextureColorizer *texColorizer) override;

private:
    void mapTexture(const ViewportParams *viewport, int tileZoomLevel, MapQuality mapQuality);

private:
    class RenderJob;

    StackedTileLoader *const m_tileLoader;
    int m_radius;
    QImage m_canvasImage;
    int m_oldYPaintedTop;
    QThreadPool m_threadPool;
};

}

#endif
