//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// Description: TextureTile contains a single image quadtile 
// and jumptables for faster access to the pixel data
//


#ifndef __MARBLE__TEXTURETILE_H
#define __MARBLE__TEXTURETILE_H


#include <QtCore/QCache>
#include <QtCore/QObject>

#include "TileId.h"

class QDateTime;
class QImage;
class QString;
class QUrl;

namespace Marble
{

class TextureTilePrivate;
class GeoSceneTexture;

class TextureTile : public QObject {
    Q_OBJECT

 public:

    enum TileState {
        TileEmpty,
        TilePartial,
        TileComplete
    };

    enum DatasetState {
        DatasetEmpty,
        DatasetZeroLevel,
        DatasetScaled,
        DatasetComplete
    };

    explicit TextureTile( TileId const& tid );

    virtual ~TextureTile();
    
    void loadDataset( Marble::GeoSceneTexture *textureLayer, int level, int x,
                      int y, QCache<TileId, TextureTile> *tileCache = 0 );

    TileId const& id() const;
    int depth() const;

    bool used() const;
    void setUsed( bool used );

    int numBytes() const;

    TileState state() const;
    void setState( TileState state );

    const QImage& rawtile();
    QImage *tile();
    const QDateTime & created() const;

    // Here we retrieve the color value of the requested pixel on the tile.
    // This needs to be done differently for grayscale ( uchar, 1 byte ).
    // and color ( uint, 4 bytes ) images.

    uint pixel( int x, int y ) const;

 Q_SIGNALS:
    void tileUpdateDone();
    void downloadTile(const QUrl& sourceUrl, const QString& destinationFileName,
                      const QString& id);

 public Q_SLOTS:
    void   loadTile( bool requestTileUpdate = true );

//  protected:
//     void     showTileId( QImage& worktile, QString theme, int level, int x, int y );

 private:
    Q_DISABLE_COPY( TextureTile )
    TextureTilePrivate * const d;

    void scaleTileFrom( Marble::GeoSceneTexture *textureLayer, QImage &tile,
                        qreal sourceX, qreal sourceY, int sourceLevel,
                        int targetX, int targetY, int targetLevel );
};

}

#endif // __MARBLE__TEXTURETILE_H
