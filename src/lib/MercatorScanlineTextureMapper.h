//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
// Copyright 2008      Inge Wallin      <inge@lysator.liu.se>
//

#ifndef MARBLE_MERCATORSCANLINETEXTUREMAPPER_H
#define MARBLE_MERCATORSCANLINETEXTUREMAPPER_H


#include "TextureMapperInterface.h"

namespace Marble
{

class MercatorScanlineTextureMapper : public TextureMapperInterface
{
    Q_OBJECT

 public:
    explicit MercatorScanlineTextureMapper( StackedTileLoader *tileLoader, QObject *parent = 0 );

    virtual void mapTexture( GeoPainter *painter,
                             ViewParams *viewParams,
                             const QRect &dirtyRect,
                             TextureColorizer *texColorizer );

    virtual void setRepaintNeeded();

 private:
    void mapTexture( ViewParams *viewParams );

 private:
    StackedTileLoader *const m_tileLoader;
    bool   m_repaintNeeded;
    int    m_oldYPaintedTop;
};

}

#endif
