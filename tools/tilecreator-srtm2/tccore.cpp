//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
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
#include <QCache>

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

        qreal startLat = ( ( ( (qreal)n * 180 ) / nmax ) - 90 ) * -1;
        qreal startLng = ( ( (qreal)m * 360 ) / mmax ) - 180;

        int startLngPxResized = c_defaultTileSize * m;
        int startLatPxResized = c_defaultTileSize * n;


        if (hgtFileName(std::floor(startLng), std::floor(startLat)).isNull()
            && hgtFileName(std::floor(startLng)+1, std::floor(startLat)).isNull()
            && hgtFileName(std::floor(startLng), std::floor(startLat)-1).isNull()
            && hgtFileName(std::floor(startLng)+1, std::floor(startLat)-1).isNull()
        ) {
            QImage ret( c_defaultTileSize, c_defaultTileSize, QImage::Format_ARGB32  );
            QPainter painter( &ret );
            painter.fillRect( 0, 0, c_defaultTileSize, c_defaultTileSize, QColor( Qt::black ) );
            return ret;
        }

        QImage image( 2400, 2400, QImage::Format_ARGB32  );
        {
            QPainter painter( &image );
            painter.fillRect( 0, 0, 2400, 2400, QColor( Qt::black ) );
            QImage i = readHgt(std::floor(startLng), std::floor(startLat));
            painter.drawImage( 0, 0, i );
            painter.drawImage( 1200, 0, readHgt( std::floor(startLng)+1, std::floor(startLat) ) );
            painter.drawImage( 0, 1200, readHgt( std::floor(startLng), std::floor(startLat)-1 ) );
            painter.drawImage( 1200, 1200, readHgt( std::floor(startLng)+1, std::floor(startLat)-1 ) );
        }

        int imageSizeResized = 2400 * (512 * c_defaultTileSize) / (1200 * 180);

        // Pick the current row and smooth scale it
        // to make it match the expected size
        image = image.scaled( QSize(imageSizeResized, imageSizeResized),
                        Qt::IgnoreAspectRatio,
                        Qt::SmoothTransformation );


        //startL??Px: position in px of what we are looking for
        int startLngPx = startLng * 1200;
        startLngPx = ( 180 * 1200 ) + startLngPx;
        //qDebug() << "startLngPx(/1200)" << (qreal)startLngPx / 1200;

        int startLatPx = startLat * 1200;
        startLatPx = ( 90 * 1200 ) - startLatPx;
        //qDebug() << "startLatPx(/1200)" << (qreal)startLatPx / 1200;


        //imageL??Px: position in px of image
        int imageLngPx = std::floor(startLng);
        imageLngPx = 180 + imageLngPx;
        //qDebug() << "imageLngPx(/1200)" << imageLngPx << "*1200" << imageLngPx*1200;
        imageLngPx *= 1200;

        int imageLatPx = std::floor(90 - startLat);
        //qDebug() << "imageLatPx(/1200)" << imageLatPx;
        imageLatPx *= 1200;

        //qDebug() << "lng" << imageLngPx << startLngPx << "offset" << startLngPx - imageLngPx;
        //qDebug() << "lat" << imageLatPx << startLatPx << "offset" << startLatPx - imageLatPx;
        Q_ASSERT(1200*2 - (startLngPx - imageLngPx) >= 675);
        Q_ASSERT(1200*2 - (startLatPx - imageLatPx) >= 675);
        Q_ASSERT(startLngPx - imageLngPx >= 0);
        Q_ASSERT(startLatPx - imageLatPx >= 0);

        int imageLngPxResized = imageLngPx * 1.6; //(512 * c_defaultTileSize) / (1200 * 180);
        int imageLatPxResized = imageLatPx * 1.6; //(512 * c_defaultTileSize) / (1200 * 180);
        //qDebug() << "lng(m)" << startLngPxResized << imageLngPxResized << "diff" << startLngPxResized - imageLngPxResized;
        //qDebug() << "lat(n)" << startLatPxResized << imageLngPxResized << "diff" << startLatPxResized - imageLatPxResized;
        Q_ASSERT(startLngPxResized - imageLngPxResized < imageSizeResized);
        Q_ASSERT(startLatPxResized - imageLatPxResized < imageSizeResized);
        Q_ASSERT(startLngPxResized - imageLngPxResized >= 0);
        Q_ASSERT(startLatPxResized - imageLatPxResized >= 0);

        QImage croppedImage = image.copy(startLngPx - imageLngPx, startLatPx - imageLatPx, 675, 675);
        QImage ret = image.copy(startLngPxResized - imageLngPxResized, startLatPxResized - imageLatPxResized, c_defaultTileSize, c_defaultTileSize);
        //qDebug() << image.size() << ret.size();
        return ret;
    }

private:
    QString hgtFileName( int lng, int lat )
    {
        QChar EW( lng >= 0 ? 'E' : 'W' );
        QChar NS( lat >= 0 ? 'N' : 'S' );

        QStringList dirs;
        dirs << "Africa" << "Australia" << "Eurasia" << "Silands" << "North_America" << "South_America";
        foreach( QString dir, dirs) {
            QString fileName = m_sourceDir + "/" + dir + "/";
            if ( lat < 0 ) lat *= -1;
            fileName += QString( "%1%2%3%4.hgt" ).arg( NS ).arg( lat<0 ? lat*-1 : lat, 2, 10, QLatin1Char('0') )
                                        .arg( EW ).arg( lng<0 ? lng*-1 : lng, 3, 10, QLatin1Char('0' ) );
            //qDebug() << fileName;

            if ( !QFile::exists( fileName ) && QFile::exists( fileName + ".zip" ) ) {
                qDebug() << "zip found, unzipping";
                QProcess p;
                p.execute("unzip", QStringList() << fileName + ".zip" );
                p.waitForFinished();
                QFile( QDir::currentPath() + "/" + QFileInfo( fileName ).fileName()).rename(fileName);
            }
            if ( QFile::exists( fileName ) ) {
                return fileName;
            }
        }

        return QString();
    }

    QImage readHgt( int lng, int lat )
    {
        static QCache<QPair<int, int>, QImage > cache( 10 );
        if ( cache.contains( qMakePair( lng, lat ) ) ) {
            return *cache[ qMakePair( lng, lat ) ];
        }
        QString fileName = hgtFileName( lng, lat );
        if ( fileName.isNull() ) {
            //qDebug() << lng << lat << "hgt file does not exist, returing null image";
            return QImage();
        } else {
            //qDebug() << lng << lat << "reading hgt file" << fileName;
        }

        QFile file( fileName );

        file.open( QIODevice::ReadOnly );
        int iLat = 0;
        int iLng = 0;

        //hgt file is 1201px large, but the last px is overlapping
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
            if ( iLng > 1200 ) { //here not 1199 but one more, because of overlapping px at the end of the line
                iLng = 0;
                iLat++;
            }
        }
        file.close();

        cache.insert( qMakePair( lng, lat ), new QImage( image ) );

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
        m_tilecreator->setTileFormat( "png" );
        m_tilecreator->setTileQuality( 100 );
        m_tilecreator->setResume( true );
        connect( m_tilecreator, SIGNAL( finished() ), this, SLOT( quit() ) );
        m_tilecreator->start();
    }
}
