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
#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtGui/QImage>
#include <QtCore/QString>

#include "SunLocator.h"
#include "TileId.h"
#include "MergedLayerDecorator.h"


class QUrl;
class GeoSceneTexture;
class TextureTile;


class TextureTile : public QObject {
    Q_OBJECT

 public:
    explicit TextureTile( TileId const& tid );

    virtual ~TextureTile();
    
    void loadRawTile( GeoSceneTexture *textureLayer, int level, int x, int y, QCache<TileId, TextureTile> *tileCache = 0 );

    TileId const& id() const  { return m_id; }
    int  depth() const        { return m_depth; }

    bool used() const         { return m_used; }
    void setUsed( bool used ) { m_used = used; }

    int numBytes() const      { return m_rawtile.numBytes(); }

    const QImage& rawtile()   { return m_rawtile; }
    QImage *tile()            { return &m_rawtile; }
    const QDateTime & created() const;

    // Here we retrieve the color value of the requested pixel on the tile.
    // This needs to be done differently for grayscale ( uchar, 1 byte ).
    // and color ( uint, 4 bytes ) images.

    uint pixel( int x, int y ) const {
        if ( m_depth == 1 || m_depth == 8 ) {
            if ( !m_isGrayscale )
                return m_rawtile.pixel( x, y );
            else
                return  jumpTable8[y][x];
        }
        return  jumpTable32[y][x];
    }

 Q_SIGNALS:
    void tileUpdateDone();
    void downloadTile(const QUrl& sourceUrl, const QString& destinationFileName,
                      const QString& id);

 public Q_SLOTS:
    void   loadTile( bool requestTileUpdate = true );

 protected:
    void     showTileId( QImage& worktile, QString theme, int level, int x, int y );

    uchar  **jumpTable8;
    uint   **jumpTable32;

    TileId   m_id;

    QImage   m_rawtile;

    int      m_depth;
    bool     m_isGrayscale;
    bool     m_used;

 private:
    Q_DISABLE_COPY( TextureTile )
    QDateTime m_created;

    void scaleTileFrom( GeoSceneTexture *textureLayer, QImage &tile, double sourceX, double sourceY, int sourceLevel, int targetX, int targetY, int targetLevel  );
};

inline const QDateTime & TextureTile::created() const
{
    return m_created;
}

#endif // __MARBLE__TEXTURETILE_H
