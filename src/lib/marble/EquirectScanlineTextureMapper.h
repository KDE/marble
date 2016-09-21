//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
// Copyright 2008      Inge Wallin      <inge@lysator.liu.se>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_EQUIRECTSCANLINETEXTUREMAPPER_H
#define MARBLE_EQUIRECTSCANLINETEXTUREMAPPER_H


#include "TextureMapperInterface.h"

#include "MarbleGlobal.h"

#include <QThreadPool>
#include <QImage>


namespace Marble
{
class StackedTileLoader;

class EquirectScanlineTextureMapper : public TextureMapperInterface
{
 public:
    explicit EquirectScanlineTextureMapper( StackedTileLoader *tileLoader );

    virtual void mapTexture( GeoPainter *painter,
                             const ViewportParams *viewport,
                             int tileZoomLevel,
                             const QRect &dirtyRect,
                             TextureColorizer *texColorizer );

 private:
    void mapTexture( const ViewportParams *viewport, int tileZoomLevel, MapQuality mapQuality );

 private:
    class RenderJob;

    StackedTileLoader *const m_tileLoader;
    int m_radius;
    QImage m_canvasImage;
    int    m_oldYPaintedTop;
    QThreadPool m_threadPool;
};

}

#endif
