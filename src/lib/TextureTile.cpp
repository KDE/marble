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
#include "TextureTile_p.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtGui/QImage>

#include <cmath>

#include "global.h"
#include "GeoSceneTexture.h"
#include "MarbleDirs.h"
#include "TileLoaderHelper.h"


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


TextureTilePrivate::TextureTilePrivate( const TileId& id ) :
      AbstractTilePrivate( id ), 
      jumpTable8(0),
      jumpTable32(0),
      m_rawtile(),
      m_depth(0),
      m_isGrayscale(false)
{
}

TextureTilePrivate::~TextureTilePrivate()
{
      delete [] jumpTable32;
      delete [] jumpTable8;
}

uint TextureTilePrivate::pixel( int x, int y ) const
{
    if ( m_depth == 8 || m_depth == 1 ) {
        if ( !m_isGrayscale )
            return m_rawtile.pixel( x, y );
        else
            return (jumpTable8)[y][x];
    }
    return (jumpTable32)[y][x];
}

uint TextureTilePrivate::pixelF( qreal x, qreal y ) const
{
    // Bilinear interpolation to determine the color of a subpixel 

    int iX = (int)(x);
    int iY = (int)(y);

    QRgb topLeftValue  =  pixel( iX, iY );

    qreal fY = y - iY;

    // Interpolation in y-direction
    if ( ( iY + 1 ) < m_rawtile.height() ) {

        QRgb bottomLeftValue  =  pixel( iX, iY + 1 );

        // blending the color values of the top left and bottom left point
        qreal ml_red   = ( 1.0 - fY ) * qRed  ( topLeftValue  ) + fY * qRed  ( bottomLeftValue  );
        qreal ml_green = ( 1.0 - fY ) * qGreen( topLeftValue  ) + fY * qGreen( bottomLeftValue  );
        qreal ml_blue  = ( 1.0 - fY ) * qBlue ( topLeftValue  ) + fY * qBlue ( bottomLeftValue  );

        // Interpolation in x-direction
        if ( iX + 1 < m_rawtile.width() ) {

            qreal fX = x - iX;

            QRgb topRightValue    =  pixel( iX + 1, iY );
            QRgb bottomRightValue =  pixel( iX + 1, iY + 1 );

            // blending the color values of the top right and bottom right point
            qreal mr_red   = ( 1.0 - fY ) * qRed  ( topRightValue ) + fY * qRed  ( bottomRightValue );
            qreal mr_green = ( 1.0 - fY ) * qGreen( topRightValue ) + fY * qGreen( bottomRightValue );
            qreal mr_blue  = ( 1.0 - fY ) * qBlue ( topRightValue ) + fY * qBlue ( bottomRightValue );
    
            // blending the color values of the resulting middle left 
            // and middle right points
            int mm_red   = (int)( ( 1.0 - fX ) * ml_red   + fX * mr_red   );
            int mm_green = (int)( ( 1.0 - fX ) * ml_green + fX * mr_green );
            int mm_blue  = (int)( ( 1.0 - fX ) * ml_blue  + fX * mr_blue  );
    
            return qRgb( mm_red, mm_green, mm_blue );
        }
        else {
            return qRgb( ml_red, ml_green, ml_blue );
        }
    }
    else {
        // Interpolation in x-direction
        if ( iX + 1 < m_rawtile.width() ) {

            qreal fX = x - iX;

            if ( fX == 0.0 ) 
                return topLeftValue;

            QRgb topRightValue    =  pixel( iX + 1, iY );

            // blending the color values of the top left and top right point
            int tm_red   = (int)( ( 1.0 - fX ) * qRed  ( topLeftValue ) + fX * qRed  ( topRightValue ) );
            int tm_green = (int)( ( 1.0 - fX ) * qGreen( topLeftValue ) + fX * qGreen( topRightValue ) );
            int tm_blue  = (int)( ( 1.0 - fX ) * qBlue ( topLeftValue ) + fX * qBlue ( topRightValue ) );

            return qRgb( tm_red, tm_green, tm_blue );
        }
    }

    return topLeftValue;
}

void TextureTilePrivate::scaleTileFrom( Marble::GeoSceneTexture *textureLayer, QImage &tile,
                    qreal sourceX, qreal sourceY, int sourceLevel,
                    int targetX, int targetY, int targetLevel )
{
    const int levelZeroColumns = textureLayer->levelZeroColumns();
    const int levelZeroRows = textureLayer->levelZeroRows();
    const int rowsRequestedLevel = TileLoaderHelper::levelToRow( levelZeroRows, targetLevel );
    const int columnsRequestedLevel = TileLoaderHelper::levelToColumn( levelZeroColumns,
                                                                       targetLevel );
    const int rowsCurrentLevel = TileLoaderHelper::levelToRow( levelZeroRows, sourceLevel );
    const int columnsCurrentLevel = TileLoaderHelper::levelToColumn( levelZeroColumns,
                                                                     sourceLevel );

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
    m_state = AbstractTile::TilePartial;
    // qDebug() << "Finished scaling up the Temporary Tile.";
}


TextureTile::TextureTile( TileId const& id, QObject * parent )
    : AbstractTile( *new TextureTilePrivate( id ), parent )
{
//    Q_D( TextureTile );
//    d->q_ptr = this; 
}

TextureTile::TextureTile( TextureTilePrivate &dd, QObject * parent )
    : AbstractTile( dd, parent )
{
}

TextureTile::~TextureTile()
{
}

void TextureTile::loadDataset( GeoSceneTexture *textureLayer, int level, int x, int y,
                               QCache<TileId, TextureTile> *tileCache )
{
    Q_D( TextureTile );

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
            QString relfilename =
                TileLoaderHelper::relativeTileFileName( textureLayer, currentLevel,
                                                        (int)(currentX), (int)(currentY) );
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
                    d->scaleTileFrom( textureLayer, temptile, currentX, currentY, currentLevel,
                                      x, y, level );
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
            QUrl sourceUrl =
                TileLoaderHelper::downloadUrl( textureLayer, currentLevel,
                                               currentX, currentY );
            QString destFileName =
                TileLoaderHelper::relativeTileFileName( textureLayer, currentLevel,
                                                        currentX, currentY );
//            qDebug() << "emit downloadTile(" << sourceUrl << destFileName << ");";
            emit downloadTile( sourceUrl, destFileName, d->m_id.toString() );
        }
    }

//    qDebug() << "TextureTile::loadDataset end";

    d->m_depth = d->m_rawtile.depth();

//    qDebug() << "m_depth =" << m_depth;
}

void TextureTile::initJumpTables( bool requestTileUpdate )
{
    Q_D( TextureTile );
    //    qDebug() << "Entered initJumpTables( bool ) of Tile" << d->m_id;

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
            qDebug() << QString("Color m_depth %1 of tile could not be retrieved. Exiting.").
                arg( d->m_depth );
            exit( -1 );
    }

    d->m_isGrayscale = d->m_rawtile.isGrayscale();

    if ( requestTileUpdate ) {
        // qDebug() << "TileUpdate available";
        emit tileUpdateDone();
    }
}

uint TextureTile::pixel( int x, int y ) const
{
    Q_D( const TextureTile );

    return d->pixel( x, y );
}

uint TextureTile::pixelF( qreal x, qreal y ) const
{
    Q_D( const TextureTile );

    return d->pixelF( x, y );
}

int TextureTile::depth() const
{
    Q_D( const TextureTile );
    return d->m_depth;
}

int TextureTile::numBytes() const
{
    Q_D( const TextureTile );
    return d->m_rawtile.numBytes();
}

QImage TextureTile::rawtile() 
{
    Q_D( TextureTile );
    return d->m_rawtile;
}

QImage * TextureTile::tile()
{
    Q_D( TextureTile );
    return &(d->m_rawtile);
}

#include "TextureTile.moc"
