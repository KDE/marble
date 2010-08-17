//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_GLTEXTUREMAPPER_H
#define MARBLE_GLTEXTUREMAPPER_H


#include "AbstractScanlineTextureMapper.h"
#include "TileId.h"

class QRect;
class QGLContext;

namespace Marble
{

class GeoPainter;
class StackedTileLoader;
class TextureColorizer;
class TileLoader;
class ViewParams;
class ViewportParams;

class GLTextureMapper : public AbstractScanlineTextureMapper
{
    Q_OBJECT

 public:
    GLTextureMapper( StackedTileLoader *tileLoader,
                     QGLContext *glContext,
                     TileLoader *srtmLoader,
                     QObject *parent = 0 );
    virtual ~GLTextureMapper();

    virtual void mapTexture( GeoPainter *painter,
                             ViewParams *viewParams,
                             const QRect &dirtyRect,
                             TextureColorizer *texColorizer );

    virtual void setRepaintNeeded();

 private Q_SLOTS:
    void setViewport( const ViewportParams *viewport );
    void updateTile( const TileId &id );

 private:
    void geoCoordinates( const qreal x, const qreal y, qreal &lon, qreal &lat ) const;
    void projectionCoordinates( qreal lon, qreal lat, qreal &x, qreal &y ) const;

 private:
    class Private;
    Private *const d;

    class Tile;
};

}

#endif
