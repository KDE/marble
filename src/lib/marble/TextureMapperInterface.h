//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de
//

#ifndef MARBLE_TEXTUREMAPPERINTERFACE_H
#define MARBLE_TEXTUREMAPPERINTERFACE_H

class QRect;

namespace Marble
{

class GeoPainter;
class TextureColorizer;
class ViewportParams;


class TextureMapperInterface
{
public:
    TextureMapperInterface();
    virtual ~TextureMapperInterface();

    virtual void mapTexture( GeoPainter *painter,
                             const ViewportParams *viewport,
                             int tileZoomLevel,
                             const QRect &dirtyRect,
                             TextureColorizer *texColorizer ) = 0;

    void setRepaintNeeded();

protected:
    bool m_repaintNeeded;
};

}

#endif
