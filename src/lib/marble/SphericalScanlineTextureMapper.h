// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_SPHERICALSCANLINETEXTUREMAPPER_H
#define MARBLE_SPHERICALSCANLINETEXTUREMAPPER_H

#include "TextureMapperInterface.h"

#include "MarbleGlobal.h"

#include <QImage>
#include <QThreadPool>

namespace Marble
{

class StackedTileLoader;

/*
 * @short Texture mapping onto a sphere
 *
 * This class provides a fast way to map textures onto a sphere
 * without making use of hardware acceleration.
 *
 * @author Torsten Rahn <rahn@kde.org>
 */

class SphericalScanlineTextureMapper : public TextureMapperInterface
{
public:
    explicit SphericalScanlineTextureMapper(StackedTileLoader *tileLoader);

    void mapTexture(GeoPainter *painter, const ViewportParams *viewport, int tileZoomLevel, const QRect &dirtyRect, TextureColorizer *texColorizer) override;

private:
    void mapTexture(const ViewportParams *viewport, int tileZoomLevel, MapQuality mapQuality);

private:
    class RenderJob;
    StackedTileLoader *const m_tileLoader;
    int m_radius;
    QImage m_canvasImage;
    QThreadPool m_threadPool;
};

}

#endif
