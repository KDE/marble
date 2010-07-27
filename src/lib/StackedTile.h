//
// This file is part of the Marble Desktop Globe.
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

#ifndef MARBLE_STACKEDTILE_H
#define MARBLE_STACKEDTILE_H

#include <QtCore/QSharedPointer>
#include <QtCore/QVector>
#include <QtGui/QColor>

#include "AbstractTile.h"
#include "global.h"

class QImage;

namespace Marble
{

class StackedTilePrivate;
class GeoSceneTexture;
class TextureTile;
class StackedTileLoader;

class StackedTile : public AbstractTile
{
    friend class StackedTileLoader;

 public:
    explicit StackedTile( TileId const& tid, QObject * parent = 0 );
    virtual ~StackedTile();

    int depth() const;
    int numBytes() const;
    bool isExpired() const;
    bool hasTiles() const;

    bool forMergedLayerDecorator() const;
    void setForMergedLayerDecorator();

    QVector<QSharedPointer<TextureTile> > * tiles();
    QImage const * resultTile() const;
    QImage * resultTile();

    // Here we retrieve the color value of the requested pixel on the tile.
    // This needs to be done differently for grayscale ( uchar, 1 byte ).
    // and color ( uint, 4 bytes ) images.

    uint pixel( int x, int y ) const;
    
    // Here we retrieve the color value of the requested subpixel on the tile.
    // This needs to be done differently for grayscale ( uchar, 1 byte ).
    // and color ( uint, 4 bytes ) images.
    // Subpixel calculation is done via bilinear interpolation.

    uint pixelF( qreal x, qreal y ) const;
    uint pixelF( qreal x, qreal y, const QRgb& pixel ) const;

 protected:
    StackedTile( StackedTilePrivate &dd, QObject *parent );

 private:
    Q_DECLARE_PRIVATE( StackedTile )
    Q_DISABLE_COPY( StackedTile )

    void addTile( QSharedPointer<TextureTile> const & );
    void initJumpTables();
    void initResultTile();

    StackedTilePrivate *d;
};

}

#endif
