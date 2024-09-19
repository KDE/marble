// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Andrew Manson <g.real.ate@gmail.com>
// SPDX-FileCopyrightText: 2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
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

    virtual void mapTexture(GeoPainter *painter, const ViewportParams *viewport, int tileZoomLevel, const QRect &dirtyRect, TextureColorizer *texColorizer) = 0;

    void setRepaintNeeded();

protected:
    bool m_repaintNeeded;
};

}

#endif
