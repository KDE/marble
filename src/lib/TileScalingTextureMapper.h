//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_TILESCALINGTEXTUREMAPPER_H
#define MARBLE_TILESCALINGTEXTUREMAPPER_H


#include "AbstractScanlineTextureMapper.h"

namespace Marble
{

class TileScalingTextureMapper : public AbstractScanlineTextureMapper
{
    Q_OBJECT

 public:
    explicit TileScalingTextureMapper( StackedTileLoader *tileLoader, QObject *parent = 0 );

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
    int m_oldYPaintedTop;
};

}

#endif
