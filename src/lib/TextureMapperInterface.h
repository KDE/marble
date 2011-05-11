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

#include <QtCore/QObject>

class QRect;

namespace Marble
{

class GeoPainter;
class StackedTile;
class StackedTileLoader;
class TextureColorizer;
class ViewParams;


class TextureMapperInterface : public QObject
{
    Q_OBJECT

public:
    explicit TextureMapperInterface( QObject * const parent = 0 );
    ~TextureMapperInterface();

    void setTileLevel( int tileLevel );

    virtual void mapTexture( GeoPainter *painter,
                             ViewParams *viewParams,
                             const QRect &dirtyRect,
                             TextureColorizer *texColorizer ) = 0;

    virtual void setRepaintNeeded() = 0;

    int tileZoomLevel() const;

 Q_SIGNALS:
    void tileUpdatesAvailable();

 private:
    Q_DISABLE_COPY( TextureMapperInterface )

    int         m_tileLevel;
};

inline int TextureMapperInterface::tileZoomLevel() const
{
    return m_tileLevel;
}

}

#endif
