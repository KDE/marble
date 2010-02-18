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


#ifndef MARBLE_STACKED_TILE_H
#define MARBLE_STACKED_TILE_H


#include <QtCore/QCache>
#include <QtCore/QObject>
#include <QtGui/QColor>

#include "AbstractTile.h"
#include "global.h"

class QImage;
class QString;
class QUrl;

namespace Marble
{

class StackedTilePrivate;
class GeoSceneTexture;
class TextureTile;
class StackedTileLoader;

class StackedTile : public AbstractTile
{
    Q_OBJECT
    friend class StackedTileLoader;

 public:
    explicit StackedTile( TileId const& tid, QObject * parent = 0 );
    virtual ~StackedTile();

    int depth() const;
    int numBytes() const;
    bool expired() const;

    bool forMergedLayerDecorator() const;
    void setForMergedLayerDecorator();

    QImage rawtile();
    QImage *tile();

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

 Q_SIGNALS:
    void downloadTile(const QUrl& sourceUrl, const QString& destinationFileName,
                      const QString& id, DownloadUsage );

 protected:
    StackedTile( StackedTilePrivate &dd, QObject *parent );

 private:
    Q_DECLARE_PRIVATE( StackedTile )
    Q_DISABLE_COPY( StackedTile )

    void addBaseTile( TextureTile * const );
    void deriveCompletionState();
    void initJumpTables();
    bool hasBaseTiles() const;
    void initResultTile();

    StackedTilePrivate *d;
};

}

#endif // MARBLE_STACKED_TILE_H
