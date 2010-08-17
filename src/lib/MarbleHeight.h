//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_MARBLEHEIGHT_H
#define MARBLE_MARBLEHEIGHT_H

#include "TileId.h"

#include <QObject>
#include <QCache>

class QImage;

namespace Marble
{

class GeoSceneTexture;
class TileLoader;


class MarbleHeight : public QObject
{
    Q_OBJECT
public:
    explicit MarbleHeight( QObject* parent = 0 );

    void setTileLoader( TileLoader *tileLoader );

    void setRadius( int radius );

    qreal altitude( qreal lon, qreal lat );

private:
    static const GeoSceneTexture *srtmLayer();

private:
    const GeoSceneTexture *const m_textureLayer;
    const uint m_hash;
    TileLoader *m_tileLoader;
    int m_radius;
    int m_level;
    int m_numXTiles;
    int m_numYTiles;
    QCache<TileId, const QImage> m_cache;
};

}

#endif
