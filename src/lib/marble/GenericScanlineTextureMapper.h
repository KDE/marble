// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_GENERICSCANLINETEXTUREMAPPER_H
#define MARBLE_GENERICSCANLINETEXTUREMAPPER_H

#include "TextureMapperInterface.h"

#include <QImage>
#include <QThreadPool>

#include <MarbleGlobal.h>

namespace Marble
{

class StackedTileLoader;

class GenericScanlineTextureMapper : public TextureMapperInterface
{
public:
    explicit GenericScanlineTextureMapper(StackedTileLoader *tileLoader);

    void mapTexture(GeoPainter *painter, const ViewportParams *viewport, int tileZoomLevel, const QRect &dirtyRect, TextureColorizer *texColorizer) override;

private:
    class RenderJob;

    void mapTexture(const ViewportParams *viewport, int tileZoomLevel, MapQuality mapQuality);

    StackedTileLoader *const m_tileLoader;
    int m_radius;
    QImage m_canvasImage;
    QThreadPool m_threadPool;
};

}

#endif
