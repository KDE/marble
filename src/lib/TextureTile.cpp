//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "TextureTile.h"

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtGui/QImage>

#include <cmath>

#include "global.h"
#include "GeoSceneTexture.h"
#include "MarbleDirs.h"
#include "TileLoaderHelper.h"

namespace Marble {

class TextureTilePrivate {
 public:
    uchar   **jumpTable8;
    uint    **jumpTable32;

    TileId    m_id;

    QImage    m_rawtile;

    int       m_depth;
    bool      m_isGrayscale;
    bool      m_used;

    TextureTile::TileState m_state;

    QDateTime m_created;

    TextureTilePrivate(const TileId& id) :
      jumpTable8(0),
      jumpTable32(0),
      m_id(id),
      m_rawtile(),
      m_depth(0),
      m_isGrayscale(false),
      m_used(false),
      m_state(TextureTile::TileEmpty),
      m_created(QDateTime::currentDateTime()) { }

    ~TextureTilePrivate() {
      delete [] jumpTable32;
      delete [] jumpTable8;
    }
};

};

using namespace Marble;

static uint **jumpTableFromQImage32( QImage &img )
{
    const int  height = img.height();
    const int  bpl    = img.bytesPerLine() / 4;
    uint      *data = (QRgb*)(img.scanLine(0));
    uint     **jumpTable = new uint*[height];

    for ( int y = 0; y < height; ++y ) {
        jumpTable[ y ] = data;
        data += bpl;
    }

    return jumpTable;
}


static uchar **jumpTableFromQImage8( QImage &img )
{
    const int  height = img.height();
    const int  bpl    = img.bytesPerLine();
    uchar     *data = img.bits();
    uchar    **jumpTable = new uchar*[height];

    for ( int y = 0; y < height; ++y ) {
        jumpTable[ y ] = data;
        data += bpl;
    }

    return jumpTable;
}

TextureTile::TextureTile( TileId const& id )
    : QObject(),
      d(new TextureTilePrivate(id))
{
}


TextureTile::~TextureTile()
{
    delete d;
}

void TextureTile::loadDataset( GeoSceneTexture *textureLayer, int level, int x, int y, QCache<TileId, TextureTile> *tileCache )
{
    // qDebug() << "TextureTile::loadDataset" << level << x << y;
    QImage temptile;

    d->m_used = true; // Needed to avoid frequent deletion of tiles

    QString  absfilename;

    // If the tile level offers the requested tile then load it.
    // Otherwise cycle from the requested tilelevel down to one where
    // the requested area is covered.  Then scale the area to create a
    // replacement for the tile that has been requested.

    const int levelZeroColumns = textureLayer->levelZeroColumns();
    const int levelZeroRows = textureLayer->levelZeroRows();
    const int rowsRequestedLevel = TileLoaderHelper::levelToRow( levelZeroRows, level );
    const int columnsRequestedLevel = TileLoaderHelper::levelToColumn( levelZeroColumns, level );
    bool tileFound = false;
    for ( int currentLevel = level; !tileFound && currentLevel > -1; --currentLevel ) {

        const int rowsCurrentLevel = 
            TileLoaderHelper::levelToRow( levelZeroRows, currentLevel );
        const int columnsCurrentLevel =
            TileLoaderHelper::levelToColumn( levelZeroColumns, currentLevel );

        qreal normalizedX = (qreal)(x) / (qreal)( rowsRequestedLevel );
        qreal normalizedY = (qreal)(y) / (qreal)( columnsRequestedLevel );
        qreal currentX    = normalizedX * (qreal)( rowsCurrentLevel );
        qreal currentY    = normalizedY * (qreal)( columnsCurrentLevel );

        const QDateTime now = QDateTime::currentDateTime();
        QDateTime lastModified;

        bool download = false;
        bool currentTileAvailable = false;

        TextureTile *currentTile = 0;
        TileId currentTileId( currentLevel, (int)(currentX), (int)(currentY) );

        // Check whether the current tile id is available in the CACHE:
        if ( tileCache ) {
            currentTile = tileCache->take( currentTileId );

            if ( currentTile ) {
                // the tile was in the cache, but is it up to date?
                lastModified = currentTile->created();
                if ( lastModified.secsTo( now ) < textureLayer->expire()) {
                    temptile = currentTile->rawtile();
                    currentTileAvailable = true;
                } else {
                    delete currentTile;
                    currentTile = 0;
                }
            }
        }
        // If the current tile id is not in the cache or if it was 
        // in the cache but has expired load from DISK:

        if ( !currentTile ) {
            QString relfilename = TileLoaderHelper::relativeTileFileName( textureLayer, 
                                                                        currentLevel, (int)(currentX), (int)(currentY) );
            absfilename = MarbleDirs::path( relfilename );
            const QFileInfo fileInfo( absfilename );
            lastModified = fileInfo.lastModified();

            // - if the file does not exist, we want to download it and search an
            //   existing tile of a lower zoom level for imediate display
            // - if the file exists and is expired according to the value of the
            //   expire element we want to download it again and display the old
            //   tile until the new one is there. Once the updated tile is
            //   available, it should get displayed.
 
            if ( !fileInfo.exists() ) {
//                qDebug() << "File does not exist:" << fileInfo.filePath();
                download = true;
            }
            else if ( lastModified.secsTo( now ) > textureLayer->expire() ) {
//                qDebug() << "File does exist, but is expired:" << fileInfo.filePath()
//                        << "age (seconds):" << lastModified.secsTo( now )
//                        << "allowed age:" << textureLayer->expire();
                download = true;
            }

            if ( fileInfo.exists() ) {

                temptile.load( absfilename );
                // qDebug() << "TextureTile::loadDataset "
                //          << "depth:" << temptile.depth()
                //          << "format:" << temptile.format()
                //          << "bytesPerLine:" << temptile.bytesPerLine()
                //          << "numBytes:" << temptile.numBytes() ;
                currentTileAvailable = true;
            }
        }

        if ( currentTileAvailable ) {
            if ( !temptile.isNull() ) {

                // Don't scale if the current tile isn't a fallback
                if ( level != currentLevel ) { 
                    scaleTileFrom( textureLayer, temptile, currentX, currentY, currentLevel, x, y, level );
                }
                else {
                    d->m_state = TileComplete;
                }

                d->m_rawtile = temptile;
                d->m_created = lastModified;
                tileFound = true;
            }
        }

        if ( download ) {
            QUrl sourceUrl = TileLoaderHelper::downloadUrl( textureLayer, level, x, y );
            QString destFileName = TileLoaderHelper::relativeTileFileName( textureLayer,
                                                                           level, x, y );
//            qDebug() << "emit downloadTile(" << sourceUrl << destFileName << ");";
            emit downloadTile( sourceUrl, destFileName, d->m_id.toString() );
        }
    }

//    qDebug() << "TextureTile::loadDataset end";

    d->m_depth = d->m_rawtile.depth();

//    qDebug() << "m_depth =" << m_depth;
}

void TextureTile::loadTile( bool requestTileUpdate )
{
    //    qDebug() << "Entered loadTile( int, int, int) of Tile" << m_id;

    if ( d->m_rawtile.isNull() ) {
        qDebug() << "An essential tile is missing. Please rerun the application.";
        exit(-1);
    }

    switch ( d->m_depth ) {
        case 48:
        case 32:
            if ( d->jumpTable32 )
                delete [] d->jumpTable32;
            d->jumpTable32 = jumpTableFromQImage32( d->m_rawtile );
            break;
        case 8:
        case 1:
            if ( d->jumpTable8 )
                delete [] d->jumpTable8;
            d->jumpTable8 = jumpTableFromQImage8( d->m_rawtile );
            break;
        default:
            qDebug() << QString("Color m_depth %1 of tile could not be retrieved. Exiting.").arg(d->m_depth);
            exit( -1 );
    }

    d->m_isGrayscale = d->m_rawtile.isGrayscale();

    if ( requestTileUpdate ) {
        // qDebug() << "TileUpdate available";
        emit tileUpdateDone();
    }
}

void TextureTile::scaleTileFrom( GeoSceneTexture *textureLayer, QImage &tile, qreal sourceX, qreal sourceY, int sourceLevel, int targetX, int targetY, int targetLevel )
{
    const int levelZeroColumns = textureLayer->levelZeroColumns();
    const int levelZeroRows = textureLayer->levelZeroRows();
    const int rowsRequestedLevel = TileLoaderHelper::levelToRow( levelZeroRows, targetLevel );
    const int columnsRequestedLevel = TileLoaderHelper::levelToColumn( levelZeroColumns, targetLevel );
    const int rowsCurrentLevel = TileLoaderHelper::levelToRow( levelZeroRows, sourceLevel );
    const int columnsCurrentLevel = TileLoaderHelper::levelToColumn( levelZeroColumns, sourceLevel );

    // qDebug() << "About to start cropping an existing image.";

    QSize tilesize = tile.size();
    qreal normalizedX2 = (qreal)(targetX + 1) / (qreal)( rowsRequestedLevel );
    qreal normalizedY2 = (qreal)(targetY + 1) / (qreal)( columnsRequestedLevel );
    qreal currentX2    = normalizedX2 * (qreal)( rowsCurrentLevel );
    qreal currentY2    = normalizedY2 * (qreal)( columnsCurrentLevel );

    // Determine the rectangular section of the previous tile data 
    // which we intend to copy from:
    int left   = (int)( ( sourceX    - (int)( sourceX ) ) * tile.width() );
    int top    = (int)( ( sourceY    - (int)( sourceY ) ) * tile.height() );
    int right  = (int)( ( currentX2  - (int)( sourceX ) ) * tile.width() ) - 1;
    int bottom = (int)( ( currentY2  - (int)( sourceY ) ) * tile.height() ) - 1;

    // Important: Scaling a null image during the next step would be fatal
    // So we make sure the width and height is at least 1.
    int rectWidth  = ( right - left > 1 ) ? right  - left : 1;
    int rectHeight = ( bottom - top > 1 ) ? bottom - top  : 1;

    // This should not create any memory leaks as
    // 'copy' and 'scaled' return a value (on the
    // stack) which gets deep copied always into the
    // same place for m_rawtile on the heap:
    tile = tile.copy( left, top, rectWidth, rectHeight );
    tile = tile.scaled( tilesize ); // TODO: use correct size
    d->m_state = TilePartial;
    // qDebug() << "Finished scaling up the Temporary Tile.";

}

const QDateTime & TextureTile::created() const
{
    return d->m_created;
}

uint TextureTile::pixel( int x, int y ) const
{
    if ( d->m_depth == 1 || d->m_depth == 8 ) {
        if ( !d->m_isGrayscale )
            return d->m_rawtile.pixel( x, y );
        else
            return (d->jumpTable8)[y][x];
    }
    return (d->jumpTable32)[y][x];
}

TileId const& TextureTile::id() const { return d->m_id; }

int TextureTile::depth() const { return d->m_depth; }

bool TextureTile::used() const { return d->m_used; }
void TextureTile::setUsed( bool used ) { d->m_used = used; }

int TextureTile::numBytes() const { return d->m_rawtile.numBytes(); }

TextureTile::TileState TextureTile::state() const { return d->m_state; }
void TextureTile::setState( TileState state ) { d->m_state = state; }

const QImage& TextureTile::rawtile() { return d->m_rawtile; }

QImage * TextureTile::tile() { return &(d->m_rawtile); }

#include "TextureTile.moc"
