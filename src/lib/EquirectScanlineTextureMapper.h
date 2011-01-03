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

#ifndef MARBLE_EQUIRECTSCANLINETEXTUREMAPPER_H
#define MARBLE_EQUIRECTSCANLINETEXTUREMAPPER_H


#include "AbstractScanlineTextureMapper.h"

namespace Marble
{

class EquirectScanlineTextureMapper : public AbstractScanlineTextureMapper
{
    Q_OBJECT

 public:
    EquirectScanlineTextureMapper( StackedTileLoader *tileLoader, QObject *parent = 0 );

    virtual void mapTexture( GeoPainter *painter,
                             ViewParams *viewParams,
                             const QRect &dirtyRect,
                             TextureColorizer *texColorizer );

    virtual void setRepaintNeeded();

 private:
    void mapTexture( ViewParams *viewParams,
                     TextureColorizer *texColorizer );

 private:
    bool   m_repaintNeeded;
    float  m_oldCenterLon;
    int    m_oldYPaintedTop;
};

}

#endif
