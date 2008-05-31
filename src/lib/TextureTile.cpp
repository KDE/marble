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

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>

#include <cmath>

#include "global.h"
#include "GeoSceneTexture.h"
#include "MarbleDirs.h"
#include "TileLoaderHelper.h"


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
      jumpTable8(0),
      jumpTable32(0),
      m_id(id),
      m_rawtile(),
      m_depth(0),
      m_isGrayscale(false),
      m_used(false),
      m_created(QDateTime::currentDateTime())
{
}


TextureTile::~TextureTile()
{
    delete [] jumpTable32;
    delete [] jumpTable8;
}

void TextureTile::loadRawTile( GeoSceneTexture *textureLayer, int level, int x, int y, QCache<TileId, TextureTile> *cache )
{
    // qDebug() << "TextureTile::loadRawTile" << level << x << y;

    m_used = true; // Needed to avoid frequent deletion of tiles

    // qDebug() << "Entered loadTile( int, int, int) of Tile" << m_id;
    // m_used = true; // Needed to avoid frequent deletion of tiles

    QString  absfilename;

    // qDebug() << "Requested tile level" << level;

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

        double normalizedX = (double)(x) / (double)( rowsRequestedLevel );
        double normalizedY = (double)(y) / (double)( columnsRequestedLevel );
        double currentX    = normalizedX * (double)( rowsCurrentLevel );
        double currentY    = normalizedY * (double)( columnsCurrentLevel );

        QString relfilename = TileLoaderHelper::relativeTileFileName( textureLayer, 
                                                                      currentLevel,
                                                                      (int)(currentX),
                                                                      (int)(currentY) );
        absfilename = MarbleDirs::path( relfilename );
        const QFileInfo fileInfo( absfilename );

        const QDateTime now = QDateTime::currentDateTime();
        bool download = false;

        // - if the file does not exist, we want to download it and search an
        //   existing tile of a lower zoom level for imediate display
        // - if the file exists and is expired according to the value of the
        //   expire element we want to download it again and display the old
        //   tile until the new one is there. Once the updated tile is
        //   available, it should get displayed.

        if ( !fileInfo.exists() ) {
            qDebug() << "File does not exist:" << fileInfo.filePath();
            download = true;
        }
        else if ( fileInfo.lastModified().secsTo( now ) > textureLayer->expire() ) {
            qDebug() << "File does exist, but is expired:" << fileInfo.filePath()
                     << "age (seconds):" << fileInfo.lastModified().secsTo( now )
                     << "allowed age:" << textureLayer->expire();
            download = true;
        }

        if ( fileInfo.exists() ) {
            // qDebug() << "The image filename does exist: " << absfilename ;

            QImage temptile( absfilename );
            // qDebug() << "TextureTile::loadRawTile "
            //          << "depth:" << temptile.depth()
            //          << "format:" << temptile.format()
            //          << "bytesPerLine:" << temptile.bytesPerLine()
            //          << "numBytes:" << temptile.numBytes() ;

            if ( !temptile.isNull() ) {
                // qDebug() << "Image has been successfully loaded.";

                if ( level != currentLevel ) { 
                    scaleTileFrom( textureLayer, temptile, currentX, currentY, currentLevel, x, y, level );
                }

                m_rawtile = temptile;
                m_created = fileInfo.lastModified();
                tileFound = true;
            }
        }

        if ( download ) {
            QUrl sourceUrl = TileLoaderHelper::downloadUrl( textureLayer, level, x, y );
            QString destFileName = TileLoaderHelper::relativeTileFileName( textureLayer,
                                                                           level, x, y );
            qDebug() << "emit downloadTile(" << sourceUrl << destFileName << ");";
            emit downloadTile( sourceUrl, destFileName, m_id.toString() );
        }
    }

    qDebug() << "TextureTile::loadRawTile end";

    m_depth = m_rawtile.depth();

    qDebug() << "m_depth =" << m_depth;
}

void TextureTile::loadTile( bool requestTileUpdate )
{
    //    qDebug() << "Entered loadTile( int, int, int) of Tile" << m_id;

    if ( m_rawtile.isNull() ) {
        qDebug() << "An essential tile is missing. Please rerun the application.";
        exit(-1);
    }

    switch ( m_depth ) {
        case 48:
        case 32:
            if ( jumpTable32 )
                delete [] jumpTable32;
            jumpTable32 = jumpTableFromQImage32( m_rawtile );
            break;
        case 8:
        case 1:
            if ( jumpTable8 )
                delete [] jumpTable8;
            jumpTable8 = jumpTableFromQImage8( m_rawtile );
            break;
        default:
            qDebug() << QString("Color m_depth %1 of tile could not be retrieved. Exiting.").arg(m_depth);
            exit( -1 );
    }

    m_isGrayscale = m_rawtile.isGrayscale();

    if ( requestTileUpdate ) {
        // qDebug() << "TileUpdate available";
        emit tileUpdateDone();
    }
}

void TextureTile::scaleTileFrom( GeoSceneTexture *textureLayer, QImage &tile, double sourceX, double sourceY, int sourceLevel, int targetX, int targetY, int targetLevel )
{
    const int levelZeroColumns = textureLayer->levelZeroColumns();
    const int levelZeroRows = textureLayer->levelZeroRows();
    const int rowsRequestedLevel = TileLoaderHelper::levelToRow( levelZeroRows, targetLevel );
    const int columnsRequestedLevel = TileLoaderHelper::levelToColumn( levelZeroColumns, targetLevel );
    const int rowsCurrentLevel = TileLoaderHelper::levelToRow( levelZeroRows, sourceLevel );
    const int columnsCurrentLevel = TileLoaderHelper::levelToColumn( levelZeroColumns, sourceLevel );

    // qDebug() << "About to start cropping an existing image.";

    QSize tilesize = tile.size();
    double normalizedX2 = (double)(targetX + 1) / (double)( rowsRequestedLevel );
    double normalizedY2 = (double)(targetY + 1) / (double)( columnsRequestedLevel );
    double currentX2    = normalizedX2 * (double)( rowsCurrentLevel );
    double currentY2    = normalizedY2 * (double)( columnsCurrentLevel );

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
    // qDebug() << "Finished scaling up the Temporary Tile.";

}

#include "TextureTile.moc"
