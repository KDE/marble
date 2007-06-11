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

#include "tilescissor.h"

#include <QtCore/QDebug>
#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtCore/QVector>
#include <QtGui/QApplication>
#include <QtGui/QImage>
#include <QtGui/QImageReader>
#include <QtGui/QPainter>

#include "katlasdirs.h"
#include "TileLoader.h"


TileScissor::TileScissor(const QString& prefix, const QString& installmap,
                         const QString& dem, const QString& targetDir) 
    : m_prefix(prefix),
      m_installmap(installmap),
      m_dem(dem),
      m_targetDir(targetDir)
{
    /* NOOP */
}


void TileScissor::createTiles()
{

    QApplication::processEvents(); 

    QString m_sourceDir = KAtlasDirs::path( "maps/earth/" + m_prefix + '/' + m_installmap );
    if ( m_targetDir.isNull() )
        m_targetDir = KAtlasDirs::localDir() + "/maps/earth/" + m_prefix + '/';
    if ( !m_targetDir.endsWith('/') )
        m_targetDir += '/';

    qDebug() << "Creating tiles from: " << m_sourceDir;
    QImageReader testimg( m_sourceDir );

    QVector<QRgb> grayScalePalette;
    for ( int cnt = 0; cnt <= 255; cnt++) {
        grayScalePalette.insert(cnt, qRgb(cnt, cnt, cnt));
    }

    int  imgw = testimg.size().width();
    int  imgh = testimg.size().height();
#if 0
      if ( imgw > 10800 || imgh > 10800 ){
          qDebug("Install map too large!");
          exit(-1);
      } 
#endif
    bool smooth = ( ( imgw % 675 ) % 2 > 0 
                    || ( imgh % 675 ) % 2 > 0 );

    int  maxtilelevel = -1;
    int  stdimgw      = 0;
    int  stdimgh      = 0;

    int count = 0;
    while ( stdimgw < imgw ) {
        maxtilelevel = count;
        stdimgw = 2 * 675 * TileLoader::levelToRow( maxtilelevel );
        count++;
    }
    stdimgh = stdimgw / 2;
    qDebug() << "Maximum tile level: " << maxtilelevel;


    if ( QDir( m_targetDir ).exists() == false ) 
        ( QDir::root() ).mkpath( m_targetDir );

    int tilelevel = 0;

    QSize stdsize = QSize( 675,675 );

    // Counting total amount of tiles to be generated for progressbar
    int maxcount = 0;

    while ( tilelevel <= maxtilelevel ) {
        int mmaxit = TileLoader::levelToColumn( tilelevel );
        for ( int m=0; m < mmaxit; m++) { 
            int nmaxit = TileLoader::levelToRow( tilelevel );
            for ( int n=0; n < nmaxit; n++)
                maxcount++;
        }
        tilelevel++;	
    }

    qDebug() << maxcount << " tiles to be created in total.";

    int mmax = TileLoader::levelToColumn( maxtilelevel );
    int nmax = TileLoader::levelToRow( maxtilelevel );

    // Loading each row at highest spatial resolution and croping tiles
    int completed = 0;
    count = 0; // reset, so that it counts the percentage correctly
    QString tilename;

    // Creating directory structure for the highest level
    QString dirname( m_targetDir
                     + QString("%1").arg(maxtilelevel) );
    if ( !QDir( dirname ).exists() ) 
        ( QDir::root() ).mkpath( dirname );

    for ( int n = 0; n < nmax; n++ ) {
        QString dirname( m_targetDir
                         + QString("%1/%2").arg(maxtilelevel).arg( n, 4, 10, QChar('0') ) );
        if ( !QDir( dirname ).exists() ) 
            ( QDir::root() ).mkpath( dirname );
    }

    for ( int n = 0; n < nmax; n++ ) {
        QApplication::processEvents(); 

        QRect   rowsrc( 0, (int)( (float)( n * imgh ) / (float)(nmax)),
                        imgw, (int)((float)(imgh) / (float)(nmax) ) );

        QImage  img( m_sourceDir );

        QImage  row = img.copy( rowsrc );
        QSize   destsize( stdimgw, 675 );

        if ( smooth ) {
            row = img.scaled( destsize,
                              Qt::IgnoreAspectRatio,
                              Qt::SmoothTransformation );
        }

        if ( row.isNull() ) 
            qDebug() << "Read-Error! Null QImage!";

        for ( int m = 0; m < mmax; m++ ) {
            QApplication::processEvents(); 

            QImage tile = row.copy( m * imgw / mmax, 0, 675, 675 );

            tilename = m_targetDir + QString("%1/%2/%2_%3.jpg").arg( maxtilelevel ).arg( n, 4, 10, QChar('0') ).arg( m, 4, 10, QChar('0') );

            if ( m_dem == "true" ) {
                tile = tile.convertToFormat(QImage::Format_Indexed8, 
                                            grayScalePalette, Qt::ThresholdDither);
            }

            bool  noerr = tile.save( tilename, "jpg", 100 );
            if ( noerr == false )
                qDebug() << "Error while writing Tile: " << tilename;

            completed = (int) ( 90 * (float)(count) / (float)(maxcount) );	
            count++;
						
            emit progress( completed );
        }
    }

    qDebug() << "Tilelevel: " << maxtilelevel << " successfully created.";

    tilelevel = maxtilelevel - 1;

    // Now that we have the tiles at the highest resolution lets build
    // them together four by four.

    while( tilelevel >= 0 ) {
        int nmaxit =  TileLoader::levelToRow( tilelevel );;

        for ( int n = 0; n < nmaxit; n++ ) {
            QString  dirname( m_targetDir
                              + QString("%1/%2").arg(tilelevel).arg( n, 4, 10, QChar('0') ) );

            // qDebug() << "dirname: " << dirname;
            if ( !QDir( dirname ).exists() ) 
                ( QDir::root() ).mkpath( dirname );

            int  mmaxit = TileLoader::levelToColumn( tilelevel );;
            for ( int m = 0; m < mmaxit; m++ ) {

                tilename = m_targetDir + QString("%1/%2/%2_%3.jpg").arg( tilelevel + 1 ).arg( 2*n, 4, 10, QChar('0') ).arg( 2*m, 4, 10, QChar('0') );
                QImage  img_topleft( tilename );
				
                tilename = m_targetDir + QString("%1/%2/%2_%3.jpg").arg( tilelevel + 1 ).arg( 2*n, 4, 10, QChar('0') ).arg( 2*m+1, 4, 10, QChar('0') );
                QImage  img_topright( tilename );

                tilename = m_targetDir + QString("%1/%2/%2_%3.jpg").arg( tilelevel + 1 ).arg( 2*n+1, 4, 10, QChar('0') ).arg( 2*m, 4, 10, QChar('0') );
                QImage  img_bottomleft( tilename );
				
                tilename = m_targetDir + QString("%1/%2/%2_%3.jpg").arg( tilelevel + 1 ).arg( 2*n+1, 4, 10, QChar('0') ).arg( 2*m+1, 4, 10, QChar('0') );
                QImage  img_bottomright( tilename );

                QImage  tile = img_topleft;

                if ( tile.depth() == 8 ) {				

                    tile.setColorTable( grayScalePalette );
                    uchar* destline;

                    for ( int y = 0; y < 338; ++y ) {
                        destline = tile.scanLine( y );
                        const uchar* srcline = img_topleft.scanLine( 2 * y );
                        for ( int x = 0; x < 338; ++x )
                            destline[x] = srcline[2*x];
                    }
                    for ( int y = 0; y < 338; ++y ) {
                        destline = tile.scanLine( y );
                        const uchar* srcline = img_topright.scanLine( 2 * y );
                        for ( int x = 338; x < 675; ++x )
                            destline[x] = srcline[2*(x-338)];		
                    }
                    for ( int y=338; y < 675; ++y ) {
                        destline = tile.scanLine( y );
                        const uchar* srcline = img_bottomleft.scanLine( 2 * ( y-338 ) );
                        for ( int x=0; x < 338; ++x )
                            destline[x]=srcline[2 * x];	
                    }
                    for ( int y = 338; y < 675; ++y ) {
                        destline = tile.scanLine( y );
                        const uchar* srcline = img_bottomright.scanLine( 2 * ( y-338 ) );
                        for ( int x = 338; x < 675; ++x )
                            destline[x] = srcline[2 * (x - 338)];
                    }
                }
                else {
                    QRgb* destline;

                    for ( int y = 0; y < 338; ++y ) {
                        destline = (QRgb*) tile.scanLine( y );
                        const QRgb* srcline = (QRgb*) img_topleft.scanLine( 2 * y );
                        for ( int x = 0; x < 338; ++x )
                            destline[x] = srcline[2 * x];
                    }
                    for ( int y = 0; y < 338; ++y ) {
                        destline = (QRgb*) tile.scanLine( y );
                        const QRgb* srcline = (QRgb*) img_topright.scanLine( 2 * y );
                        for ( int x = 338; x < 675; ++x )
                            destline[x] = srcline[2 * (x - 338)];		
                    }
                    for ( int y = 338; y < 675; ++y ) {
                        destline = (QRgb*) tile.scanLine( y );
                        const QRgb* srcline = (QRgb*) img_bottomleft.scanLine( 2 * ( y-338 ) );
                        for ( int x = 0; x < 338; ++x )
                            destline[x] = srcline[2*x];	
                    }
                    for ( int y = 338; y < 675; ++y ) {
                        destline = (QRgb*) tile.scanLine( y );
                        const QRgb* srcline = (QRgb*) img_bottomright.scanLine( 2 * ( y-338 ) );
                        for ( int x = 338; x < 675; ++x )
                            destline[x] = srcline[2*(x-338)];		
                    }
                }

                tilename = m_targetDir + QString("%1/%2/%2_%3.jpg").arg( tilelevel ).arg( n, 4, 10, QChar('0') ).arg( m, 4, 10, QChar('0') );
                bool noerr = tile.save( tilename, "jpg", 100 );
                if ( noerr == false ) 
                    qDebug() << "Error while writing Tile: " << tilename;;

                completed = (int) ( 90 * (float)(count) / (float)(maxcount) );	
                count++;
						
                emit progress( completed );
                QApplication::processEvents(); 
            }
        }
        qDebug() << "Tilelevel: " << tilelevel << " successfully created.";

        tilelevel--;
    }
    qDebug() << "Tile creation completed.";

    // Applying correct JPEG compression
    int microcount = 0;
    int microcompleted = 0;

    tilelevel = 0;
    while ( tilelevel <= maxtilelevel ) {
        int nmaxit =  TileLoader::levelToRow( tilelevel );
        for ( int n = 0; n < nmaxit; n++) {
            int mmaxit =  TileLoader::levelToColumn( tilelevel );
            for ( int m = 0; m < mmaxit; m++) { 
                microcount++;

                tilename = m_targetDir + QString("%1/%2/%2_%3.jpg").arg( tilelevel ).arg( n, 4, 10, QChar('0') ).arg( m, 4, 10, QChar('0') );
                QImage tile( tilename );

                bool noerr = tile.save( tilename, "jpg", 85 );
                if ( noerr == false )
                    qDebug() << "Error while writing Tile: " << tilename; 

                microcompleted = (int) ( 100 * (float)(microcount) / (float)(maxcount) );	
                emit progress( 90 + (int)( 0.1 * (float)microcompleted ) );
                QApplication::processEvents(); 
            }
        }
        tilelevel++;	
    }

    completed = 100;
    emit progress( 100 );
    QApplication::processEvents(); 
}


#include "tilescissor.moc"
