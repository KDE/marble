//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007-2009  Torsten Rahn <tackat@kde.org>"
// Copyright 2007       Inge Wallin  <ingwa@kde.org>"
//

//
// Description: TextureTile contains a single image quadtile 
// and jumptables for faster access to the pixel data
//


#ifndef __MARBLE__TEXTURETILE_P_H
#define __MARBLE__TEXTURETILE_P_H

#include "AbstractTile_p.h"

#include <QtGui/QImage>

namespace Marble
{


class TextureTilePrivate : AbstractTilePrivate {
    Q_DECLARE_PUBLIC( TextureTile )

 public:
    uchar   **jumpTable8;
    uint    **jumpTable32;

    QImage    m_rawtile;

    int       m_depth;
    bool      m_isGrayscale;

    TextureTilePrivate( const TileId& id );
    virtual ~TextureTilePrivate();

    uint pixel( int x, int y ) const;

    void scaleTileFrom( Marble::GeoSceneTexture *textureLayer, QImage &tile,
                        qreal sourceX, qreal sourceY, int sourceLevel,
                        int targetX, int targetY, int targetLevel );
};

}

#endif // __MARBLE__TEXTURETILE_P_H
