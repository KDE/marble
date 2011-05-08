//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//

#include "tccore.h"
#include <TileLoaderHelper.h>
#include <global.h>
#include <QFile>
#include <cmath>
#include <QPainter>
#include <QProcess>
#include <QFileInfo>
#include <QDir>

using namespace Marble;

class TileCreatorSourceSrtm : public TileCreatorSource
{
public:
    TileCreatorSourceSrtm( const QString &sourceDir )
        : m_sourceDir( sourceDir )
    {
    }

    virtual QString sourcePath() const
    {
        return m_sourceDir;
    }

    virtual QSize fullImageSize() const
    {
        return QSize( 512*c_defaultTileSize*2, 512*c_defaultTileSize ); //512: 2**9 (9 zoom levels)
    }

    virtual QImage tile( int n, int m, int maxTileLevel )
    {
        Q_ASSERT( maxTileLevel == 9 );

        int  nmax = TileLoaderHelper::levelToRow( defaultLevelZeroRows, maxTileLevel );
        Q_ASSERT( nmax == 512 );
        int  mmax = TileLoaderHelper::levelToColumn( defaultLevelZeroColumns, maxTileLevel );

        //int  stdImageHeight  = nmax * c_defaultTileSize;
        //int  stdImageWidth  = mmax * c_defaultTileSize;
//n(lat)  m(lng)
//47      13

// for ( int n = 0; n < nmax; ++n ) {
//     for ( int m = 0; m < mmax; ++m ) {

        qDebug() << "**************************";
        qDebug() << "n" << n << "m" << m;

        qreal startLat = ( ( ( (qreal)n * 180 ) / nmax ) - 90 ) * -1;
        qreal startLng = ( ( (qreal)m * 360 ) / mmax ) - 180;
        qDebug() << "lat(n)(47)" << startLat;
        qDebug() << "lng(m)(13)" << startLng;

        int startLngPxResized = c_defaultTileSize * m;
        int startLatPxResized = c_defaultTileSize * n;


        QImage image( 2400, 2400, QImage::Format_ARGB32  );
        {
            QPainter painter( &image );
            QImage i = readHgt(std::floor(startLng), std::floor(startLat));
            painter.drawImage( 0, 0, i );
            painter.drawImage( 1200, 0, readHgt( std::floor(startLng)+1, std::floor(startLat) ) );
            painter.drawImage( 0, 1200, readHgt( std::floor(startLng), std::floor(startLat)-1 ) );
            painter.drawImage( 1200, 1200, readHgt( std::floor(startLng)+1, std::floor(startLat)-1 ) );
        }
        qDebug() << "got it";


        int imageSizeResized = 2400 * (512 * c_defaultTileSize) / (1200 * 180);

        // Pick the current row and smooth scale it
        // to make it match the expected size
        image = image.scaled( QSize(imageSizeResized, imageSizeResized),
                        Qt::IgnoreAspectRatio,
                        Qt::SmoothTransformation );


        //startL??Px: position in pixeln von dem was wir brauchen
        int startLngPx = startLng * 1200;
        startLngPx = ( 180 * 1200 ) + startLngPx;
        qDebug() << "startLngPx(/1200)" << (qreal)startLngPx / 1200;

        int startLatPx = startLat * 1200;
        startLatPx = ( 90 * 1200 ) - startLatPx;
        qDebug() << "startLatPx(/1200)" << (qreal)startLatPx / 1200;


        //imageL??Px: position in pixeln von image
        int imageLngPx = std::floor(startLng);
        imageLngPx = 180 + imageLngPx;
        qDebug() << "imageLngPx(/1200)" << imageLngPx << "*1200" << imageLngPx*1200;
        imageLngPx *= 1200;

        int imageLatPx = std::floor(90 - startLat);
        qDebug() << "imageLatPx(/1200)" << imageLatPx;
        imageLatPx *= 1200;

        qDebug() << "lng" << imageLngPx << startLngPx << "offset" << startLngPx - imageLngPx;
        qDebug() << "lat" << imageLatPx << startLatPx << "offset" << startLatPx - imageLatPx;
        Q_ASSERT(1200*2 - (startLngPx - imageLngPx) >= 675);
        Q_ASSERT(1200*2 - (startLatPx - imageLatPx) >= 675);
        Q_ASSERT(startLngPx - imageLngPx >= 0);
        Q_ASSERT(startLatPx - imageLatPx >= 0);

        int imageLngPxResized = imageLngPx * 1.6; //(512 * c_defaultTileSize) / (1200 * 180);
        int imageLatPxResized = imageLatPx * 1.6; //(512 * c_defaultTileSize) / (1200 * 180);
        qDebug() << "lng(m)" << startLngPxResized << imageLngPxResized << "diff" << startLngPxResized - imageLngPxResized;
        qDebug() << "lat(n)" << startLatPxResized << imageLngPxResized << "diff" << startLatPxResized - imageLatPxResized;
        Q_ASSERT(startLngPxResized - imageLngPxResized < imageSizeResized);
        Q_ASSERT(startLatPxResized - imageLatPxResized < imageSizeResized);
        Q_ASSERT(startLngPxResized - imageLngPxResized >= 0);
        Q_ASSERT(startLatPxResized - imageLatPxResized >= 0);

        QImage croppedImage = image.copy(startLngPx - imageLngPx, startLatPx - imageLatPx, 675, 675);
        QImage ret = image.copy(startLngPxResized - imageLngPxResized, startLatPxResized - imageLatPxResized, c_defaultTileSize, c_defaultTileSize);
        qDebug() << image.size() << ret.size();
//     }
// }
//         Q_ASSERT(false);
        return ret;
//         return QImage();
    }

private:
    QImage readHgt( int lng, int lat )
    {
        QChar EW( lng >= 0 ? 'E' : 'W' );
        QChar NS( lat >= 0 ? 'N' : 'S' );

        QString fileName = m_sourceDir; //"/home/niko/kdesvn/srtm2postgis/data/";
        fileName += "Eurasia/"; //TODO there is more than that
        if ( lat < 0 ) lat *= -1;
        fileName += QString( "%1%2%3%4.hgt" ).arg( NS ).arg( lat, 2, 10, QLatin1Char('0') )
                                    .arg( EW ).arg( lng, 3, 10, QLatin1Char('0' ) );
        qDebug() << fileName;
        QFile file( fileName );
        if ( !file.exists() && QFile::exists( fileName + ".zip" ) ) {
            qDebug() << "zip found, unzipping";
            QProcess p;
            p.setWorkingDirectory( QFileInfo(fileName).dir().absolutePath() );
            p.setWorkingDirectory( "/home/niko/kdesvn/srtm2postgis/data/Eurasia" );
            p.execute("unzip", QStringList() << fileName + ".zip" );
            qDebug() << QFile( QDir::currentPath() + "/" + QFileInfo( fileName ).fileName()).fileName();
            Q_ASSERT( QFile( QDir::currentPath() + "/" + QFileInfo( fileName ).fileName()).rename(fileName) );
            p.execute("pwd");
            
            p.waitForFinished();
        }
        if (!file.exists() ) {
            qDebug() << "hgt file does not exist, returing null image";
            return QImage();
        }
    //     qDebug() << fileName;
        file.open( QIODevice::ReadOnly );
        int iLat = 0;
        int iLng = 0;

        //eigentlich ist es 1201 groß, aber das letze pixel ist überlappung glaub ich
        QImage image( 1200, 1200, QImage::Format_ARGB32 );
        while(true) {
            QByteArray data = file.read( 2 );

            if ( iLng < 1200 ) {
                unsigned short height =  *(unsigned short*)data.data();
                height = ( height << 8 | height >> 8 );
                unsigned int pixel;
                pixel = height;
                pixel += 0xFF000000; //fully opaque
                image.setPixel( iLng, iLat, pixel );
            }

            if ( iLat >= 1199 && iLng >= 1199 ) break;

            iLng++;
            if ( iLng > 1200 ) { //hier nicht nur bis 1199 sondern eins mehr, wegen dem letzten überlappungspixel
                iLng = 0;
                iLat++;
            }
        }
        file.close();

        return image;
    }

    QString m_sourceDir;
};

TCCoreApplication::TCCoreApplication( int argc, char ** argv ) : QCoreApplication( argc, argv )
{

    if( !(argc < 2) )
    {
        TileCreatorSource *source = new TileCreatorSourceSrtm( argv[1] );
        m_tilecreator = new TileCreator( source, "false", argv[2] );
        connect( m_tilecreator, SIGNAL( finished() ), this, SLOT( quit() ) );
        m_tilecreator->start();
    }
}
