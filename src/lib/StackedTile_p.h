//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007-2009  Torsten Rahn <tackat@kde.org>
// Copyright 2007       Inge Wallin  <ingwa@kde.org>
//

//
// Description: StackedTile contains a single image quadtile 
// and jumptables for faster access to the pixel data
//

#ifndef MARBLE_STACKEDTILE_P_H
#define MARBLE_STACKEDTILE_P_H

#include "TileId.h"

#include <QtCore/QSharedPointer>
#include <QtCore/QVector>
#include <QtGui/QImage>

namespace Marble
{
class Tile;

class StackedTilePrivate
{
 public:
    const TileId    m_id;
    const QImage    m_resultTile;
    const int       m_depth;
    const bool      m_isGrayscale;
    const QVector<QSharedPointer<Tile> > m_tiles;
    const uchar   **const jumpTable8;
    const uint    **const jumpTable32;
    const int m_byteCount;
    bool            m_isUsed;

    explicit StackedTilePrivate( const TileId &id, const QImage &resultImage, QVector<QSharedPointer<Tile> > const &tiles );
    virtual ~StackedTilePrivate();

    inline uint pixel( int x, int y ) const;
    inline uint pixelF( qreal x, qreal y, const QRgb& pixel ) const;
    static int calcByteCount( const QImage &resultImage, const QVector<QSharedPointer<Tile> > &tiles );
};

}

#endif
