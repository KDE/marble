//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_TILESCALINGTEXTUREMAPPER_H
#define MARBLE_TILESCALINGTEXTUREMAPPER_H


#include <QtCore/QObject>
#include "marble_export.h"
#include "TextureMapperInterface.h"

#include "TileId.h"

#include <QtCore/QCache>
#include <QtGui/QImage>
#include <QtGui/QPixmap>

namespace Marble
{

class MARBLE_EXPORT TileScalingTextureMapper : public QObject, public TextureMapperInterface
{
    Q_OBJECT

 public:
    TileScalingTextureMapper( StackedTileLoader *tileLoader, QObject *parent = 0 );

    virtual void mapTexture( GeoPainter *painter,
                             const ViewportParams *viewport,
                             const QRect &dirtyRect,
                             TextureColorizer *texColorizer );

    virtual void setRepaintNeeded();

 private Q_SLOTS:
    void removePixmap( const TileId &tileId );
    void clearPixmaps();

 private:
    void mapTexture( GeoPainter *painter,
                     const ViewportParams *viewport,
                     TextureColorizer *texColorizer );

 private:
    StackedTileLoader *const m_tileLoader;
    QCache<TileId, const QPixmap> m_cache;
    bool   m_repaintNeeded;
    QImage m_canvasImage;
    int    m_radius;
};

}

#endif
