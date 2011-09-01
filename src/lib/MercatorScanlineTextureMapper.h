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

#ifndef MARBLE_MERCATORSCANLINETEXTUREMAPPER_H
#define MARBLE_MERCATORSCANLINETEXTUREMAPPER_H


#include "TextureMapperInterface.h"

#include "global.h"

#include <QtCore/QThreadPool>
#include <QtGui/QImage>


namespace Marble
{

class MercatorScanlineTextureMapper : public TextureMapperInterface
{
    Q_OBJECT

 public:
    explicit MercatorScanlineTextureMapper( StackedTileLoader *tileLoader, QObject *parent = 0 );

    virtual void mapTexture( GeoPainter *painter,
                             const ViewportParams *viewport,
                             const QRect &dirtyRect,
                             TextureColorizer *texColorizer );

    virtual void setRepaintNeeded();

 private:
    void mapTexture( const ViewportParams *viewport, MapQuality mapQuality );

 private:
    class RenderJob;

    StackedTileLoader *const m_tileLoader;
    bool   m_repaintNeeded;
    int m_radius;
    QImage m_canvasImage;
    int    m_oldYPaintedTop;
    QThreadPool m_threadPool;
};

}

#endif
