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

#include "AbstractTile_p.h"

#include <QtCore/QSharedPointer>
#include <QtCore/QVector>
#include <QtGui/QImage>

namespace Marble
{
class TextureTile;

class StackedTilePrivate : AbstractTilePrivate
{
    Q_DECLARE_PUBLIC( StackedTile )

 public:
    uchar   **jumpTable8;
    uint    **jumpTable32;

    QVector<QSharedPointer<TextureTile> > m_tiles;
    QImage    m_resultTile;
    int m_byteCount;

    int       m_depth;
    bool      m_isGrayscale;
    bool      m_forMergedLayerDecorator;

    explicit StackedTilePrivate( const TileId& id );
    virtual ~StackedTilePrivate();

    inline uint pixel( int x, int y ) const;
    inline uint pixelF( qreal x, qreal y, const QRgb& pixel ) const;
    void setResultTile( QSharedPointer<TextureTile> const & tile,
                        const bool withConversion = true );
    void calcByteCount();
};

}

#endif
