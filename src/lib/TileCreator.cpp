//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "TileCreator.h"

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtCore/QVector>
#include <QtGui/QApplication>
#include <QtGui/QImage>
#include <QtGui/QImageReader>
#include <QtGui/QPainter>

#include "MarbleDirs.h"
#include "TileLoader.h"
#include "TextureTile.h"

// FIXME: This shouldn't be defined here, but centrally somewhere
const uint  tileSize = 675;


TileCreator::TileCreator(const QString& prefix, const QString& installmap,
                         const QString& dem, const QString& targetDir) 
    : QThread(0),
      m_prefix(prefix),
      m_installmap(installmap),
      m_dem(dem),
      m_targetDir(targetDir),
      m_cancelled( false )
{
    setTerminationEnabled( true );
}

void TileCreator::cancelTileCreation()
{
    m_cancelled = true;
}

void TileCreator::run()
{
    QString m_sourceDir = MarbleDirs::path( "maps/earth/" + m_prefix + '/' + m_installmap );
    if ( m_targetDir.isNull() )
        m_targetDir = MarbleDirs::localPath() + "/maps/earth/" + m_prefix + '/';
    if ( !m_targetDir.endsWith('/') )
        m_targetDir += '/';

    qDebug() << "Creating tiles from: " << m_sourceDir;
    QImageReader testImage( m_sourceDir );

    QVector<QRgb> grayScalePalette;
    for (unsigned int cnt = 0; cnt <= 255; ++cnt) {
        grayScalePalette.insert(cnt, qRgb(cnt, cnt, cnt));
    }

    unsigned int  imageWidth  = testImage.size().width();
    unsigned int  imageHeight = testImage.size().height();

    qDebug() << QString( "TileCreator::createTiles() image dimensions %1 x %2").arg(imageWidth).arg(imageHeight);

    if ( imageWidth > 21600 || imageHeight > 10800 ){
        qDebug("Install map too large!");
        return;
    } 

    // Calculating Maximum Tile Level
    float approxMaxTileLevel = log( imageWidth / ( 2. * tileSize ) ) / log( 2. );

    int  maxTileLevel = 0;
    if ( approxMaxTileLevel == int( approxMaxTileLevel ) )
        maxTileLevel = static_cast<int>( approxMaxTileLevel );
    else
        maxTileLevel = static_cast<int>( approxMaxTileLevel + 1 );

    if ( maxTileLevel < 0 )
    {
        qDebug() 
        << QString( "TileCreator::createTiles(): Invalid Maximum Tile Level: %1" )
        .arg( maxTileLevel );
    }
    qDebug() << "Maximum Tile Level: " << maxTileLevel;

    uint maxRows = TileLoader::levelToRow( maxTileLevel );

    // If the image size of the image source does not match the expected 
    // geometry we need to smooth-scale the image in advance to match
    // the required size 
    bool needsScaling = ( imageWidth != 2 * maxRows * tileSize
                          ||  imageHeight != maxRows * tileSize );

    if ( needsScaling ) qDebug() << "Image Size doesn't match 2*n*TILEWIDTH x n*TILEHEIGHT geometry. Scaling ...";  

    unsigned int  stdImageWidth  = 2 * maxRows * tileSize;
    if ( stdImageWidth == 0 ) stdImageWidth = 2 * tileSize;
    unsigned int  stdImageHeight  = maxRows * tileSize;

    if ( stdImageWidth != imageWidth )
    {
        qDebug() << 
        QString( "TileCreator::createTiles() The size of the final image will measure  %1 x %2 pixels").arg(stdImageWidth).arg(stdImageHeight);
    }

    if ( QDir( m_targetDir ).exists() == false ) 
        ( QDir::root() ).mkpath( m_targetDir );

    // Counting total amount of tiles to be generated for the progressbar
    // to prevent compiler warnings this var should
    // match the type of maxTileLevel
    int tileLevel = 0;
    int totalTileCount = 0;

    while ( tileLevel <= maxTileLevel ) {
        totalTileCount += TileLoader::levelToRow( tileLevel )
                * TileLoader::levelToColumn( tileLevel );
        tileLevel++;
    }

    qDebug() << totalTileCount << " tiles to be created in total.";

    unsigned int mmax = TileLoader::levelToColumn( maxTileLevel );
    unsigned int nmax = TileLoader::levelToRow( maxTileLevel );

    // Loading each row at highest spatial resolution and croping tiles
    unsigned int percentCompleted = 0;
    unsigned int createdTilesCount = 0;
    QString tileName;

    // Creating directory structure for the highest level
    QString dirName( m_targetDir
                     + QString("%1").arg(maxTileLevel) );
    if ( !QDir( dirName ).exists() ) 
        ( QDir::root() ).mkpath( dirName );

    for ( unsigned int n = 0; n < nmax; ++n ) {
        QString dirName( m_targetDir
                         + QString("%1/%2").arg(maxTileLevel).arg( n, tileDigits, 10, QChar('0') ) );
        if ( !QDir( dirName ).exists() ) 
            ( QDir::root() ).mkpath( dirName );
    }

    QImage  sourceImage( m_sourceDir );

    for ( unsigned int n = 0; n < nmax; ++n ) {
        QRect   sourceRowRect( 0, (int)( (double)( n * imageHeight ) / (double)( nmax )),
                       imageWidth,(int)( (double)( imageHeight ) / (double)( nmax ) ) );


        QImage  row = sourceImage.copy( sourceRowRect );

        if ( needsScaling ) {
            // Pick the current row and smooth scale it 
            // to make it match the expected size
            QSize destSize( stdImageWidth, tileSize );
            row = row.scaled( destSize,
                              Qt::IgnoreAspectRatio,
                              Qt::SmoothTransformation );
        }

        if ( row.isNull() ) 
        {
            qDebug() << "Read-Error! Null QImage!";
            return;
        }

        for ( unsigned int m = 0; m < mmax; ++m ) {

            if ( m_cancelled == true ) return;

            QImage tile = row.copy( m * stdImageWidth / mmax, 0, tileSize, tileSize );

            tileName = m_targetDir + QString("%1/%2/%2_%3.jpg").arg( maxTileLevel ).arg( n, tileDigits, 10, QChar('0') ).arg( m, tileDigits, 10, QChar('0') );

            if ( m_dem == "true" ) {
                tile = tile.convertToFormat(QImage::Format_Indexed8, 
                                            grayScalePalette, Qt::ThresholdDither);
            }

            bool  noerr = tile.save( tileName, "jpg", 100 );
            if ( noerr == false )
                qDebug() << "Error while writing Tile: " << tileName;

            percentCompleted =  (int) ( 90 * (double)(createdTilesCount) / 
                                (double)(totalTileCount) );	
            createdTilesCount++;
						
            emit progress( percentCompleted );
        }
    }

    qDebug() << "tileLevel: " << maxTileLevel << " successfully created.";

    tileLevel = maxTileLevel;

    // Now that we have the tiles at the highest resolution lets build
    // them together four by four.

    while( tileLevel > 0 ) {
        tileLevel--;

        unsigned int nmaxit =  TileLoader::levelToRow( tileLevel );;

        for ( unsigned int n = 0; n < nmaxit; ++n ) {
            QString  dirName( m_targetDir
                              + QString("%1/%2").arg(tileLevel).arg( n, tileDigits, 10, QChar('0') ) );

            // qDebug() << "dirName: " << dirName;
            if ( !QDir( dirName ).exists() ) 
                ( QDir::root() ).mkpath( dirName );

            unsigned int  mmaxit = TileLoader::levelToColumn( tileLevel );;
            for ( unsigned int m = 0; m < mmaxit; ++m ) {

                if ( m_cancelled == true ) return;

                tileName = m_targetDir + QString("%1/%2/%2_%3.jpg").arg( tileLevel + 1 ).arg( 2*n, tileDigits, 10, QChar('0') ).arg( 2*m, tileDigits, 10, QChar('0') );
                QImage  img_topleft( tileName );
				
                tileName = m_targetDir + QString("%1/%2/%2_%3.jpg").arg( tileLevel + 1 ).arg( 2*n, tileDigits, 10, QChar('0') ).arg( 2*m+1, tileDigits, 10, QChar('0') );
                QImage  img_topright( tileName );

                tileName = m_targetDir + QString("%1/%2/%2_%3.jpg").arg( tileLevel + 1 ).arg( 2*n+1, tileDigits, 10, QChar('0') ).arg( 2*m, tileDigits, 10, QChar('0') );
                QImage  img_bottomleft( tileName );
				
                tileName = m_targetDir + QString("%1/%2/%2_%3.jpg").arg( tileLevel + 1 ).arg( 2*n+1, tileDigits, 10, QChar('0') ).arg( 2*m+1, tileDigits, 10, QChar('0') );
                QImage  img_bottomright( tileName );

                QImage  tile = img_topleft;

                if ( tile.depth() == 8 ) {				

                    tile.setColorTable( grayScalePalette );
                    uchar* destLine;

                    for ( unsigned int y = 0; y < tileSize / 2; ++y ) {
                        destLine = tile.scanLine( y );
                        const uchar* srcLine = img_topleft.scanLine( 2 * y );
                        for ( unsigned int x = 0; x < tileSize / 2; ++x )
                            destLine[x] = srcLine[ 2*x ];
                    }
                    for ( unsigned int y = 0; y < tileSize / 2; ++y ) {
                        destLine = tile.scanLine( y );
                        const uchar* srcLine = img_topright.scanLine( 2 * y );
                        for ( uint x = tileSize / 2; x < tileSize; ++x )
                            destLine[x] = srcLine[ 2 * ( x - tileSize / 2 ) ];		
                    }
                    for ( uint y = tileSize / 2; y < tileSize; ++y ) {
                        destLine = tile.scanLine( y );
                        const uchar* srcLine = img_bottomleft.scanLine( 2 * ( y - tileSize / 2 ) );
                        for ( uint x = 0; x < tileSize / 2; ++x )
                            destLine[ x ] = srcLine[ 2 * x ];	
                    }
                    for ( uint y = tileSize / 2; y < tileSize; ++y ) {
                        destLine = tile.scanLine( y );
                        const uchar* srcLine = img_bottomright.scanLine( 2 * ( y - tileSize/2 ) );
                        for ( uint x = tileSize / 2; x < tileSize; ++x )
                            destLine[x] = srcLine[ 2 * ( x - tileSize / 2 ) ];
                    }
                }
                else {
                    QRgb* destLine;

                    for ( unsigned int y = 0; y < tileSize / 2; ++y ) {
                        destLine = (QRgb*) tile.scanLine( y );
                        const QRgb* srcLine = (QRgb*) img_topleft.scanLine( 2 * y );
                        for ( uint x = 0; x < tileSize / 2; ++x )
                            destLine[x] = srcLine[ 2 * x ];
                    }
                    for ( unsigned int y = 0; y < tileSize / 2; ++y ) {
                        destLine = (QRgb*) tile.scanLine( y );
                        const QRgb* srcLine = (QRgb*) img_topright.scanLine( 2 * y );
                        for ( uint x = tileSize / 2; x < tileSize; ++x )
                            destLine[x] = srcLine[ 2 * ( x - tileSize / 2 ) ];		
                    }
                    for ( uint y = tileSize / 2; y < tileSize; ++y ) {
                        destLine = (QRgb*) tile.scanLine( y );
                        const QRgb* srcLine = (QRgb*) img_bottomleft.scanLine( 2 * ( y-tileSize/2 ) );
                        for ( uint x = 0; x < tileSize / 2; ++x )
                            destLine[x] = srcLine[ 2 * x ];	
                    }
                    for ( uint y = tileSize / 2; y < tileSize; ++y ) {
                        destLine = (QRgb*) tile.scanLine( y );
                        const QRgb* srcLine = (QRgb*) img_bottomright.scanLine( 2 * ( y-tileSize / 2 ) );
                        for ( uint x = tileSize / 2; x < tileSize; ++x )
                            destLine[x] = srcLine[ 2*( x-tileSize / 2 ) ];		
                    }
                }

                tileName = m_targetDir + QString("%1/%2/%2_%3.jpg").arg( tileLevel ).arg( n, tileDigits, 10, QChar('0') ).arg( m, tileDigits, 10, QChar('0') );

                // Saving at 100% JPEG quality to have a high-quality version to 
                // create the remaining needed tiles from

                bool noerr = tile.save( tileName, "jpg", 100 );
                if ( noerr == false ) 
                    qDebug() << "Error while writing Tile: " << tileName;;

                percentCompleted =  (int) ( 90 * (double)(createdTilesCount) / 
                                    (double)(totalTileCount) );	
                createdTilesCount++;
						
                emit progress( percentCompleted );
            }
        }
        qDebug() << "tileLevel: " << tileLevel << " successfully created.";
    }
    qDebug() << "Tile creation completed.";

    // Applying correct lower JPEG compression now that we created all tiles

    int savedTilesCount = 0;
 
    tileLevel = 0;
    while ( tileLevel <= maxTileLevel ) {
        unsigned int nmaxit =  TileLoader::levelToRow( tileLevel );
        for ( unsigned int n = 0; n < nmaxit; ++n) {
            unsigned int mmaxit =  TileLoader::levelToColumn( tileLevel );
            for ( unsigned int m = 0; m < mmaxit; ++m) { 

                if ( m_cancelled == true ) return;

                savedTilesCount++;

                tileName = m_targetDir + QString("%1/%2/%2_%3.jpg").arg( tileLevel ).arg( n, tileDigits, 10, QChar('0') ).arg( m, tileDigits, 10, QChar('0') );
                QImage tile( tileName );

                bool noerr = tile.save( tileName, "jpg", 85 );
                if ( noerr == false )
                    qDebug() << "Error while writing Tile: " << tileName; 
                // Don't exceed 99% as this would cancel the thread unexpectedly
                percentCompleted = 90 + (int)( 9 * (double)(savedTilesCount) / 
                                   (double)(totalTileCount) );	
                emit progress( percentCompleted );
//		qDebug() << "Saving Tile #" << savedTilesCount << " of " << totalTileCount << " Percent: " << percentCompleted;
            }
        }
        tileLevel++;	
    }

    percentCompleted = 100;
    emit progress( percentCompleted );

	qDebug() << "percentCompleted: " << percentCompleted;
}


#include "TileCreator.moc"
