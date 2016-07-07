//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_GENERICSCANLINETEXTUREMAPPER_H
#define MARBLE_GENERICSCANLINETEXTUREMAPPER_H


#include "TextureMapperInterface.h"

#include <QThreadPool>
#include <QImage>

#include <MarbleGlobal.h>


namespace Marble
{

class StackedTileLoader;

class GenericScanlineTextureMapper : public TextureMapperInterface
{
 public:
    explicit GenericScanlineTextureMapper( StackedTileLoader *tileLoader );

    virtual void mapTexture( GeoPainter *painter,
                             const ViewportParams *viewport,
                             int tileZoomLevel,
                             const QRect &dirtyRect,
                             TextureColorizer *texColorizer );

 private:
    class RenderJob;

    void mapTexture( const ViewportParams *viewport, int tileZoomLevel, MapQuality mapQuality );

    StackedTileLoader *const m_tileLoader;
    int m_radius;
    QImage m_canvasImage;
    QThreadPool m_threadPool;
};

}

#endif
