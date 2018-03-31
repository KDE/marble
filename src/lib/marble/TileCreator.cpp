//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//

#include "TileCreator.h"

#include <cmath>

#include <QDir>
#include <QRect>
#include <QSize>
#include <QVector>
#include <QApplication>
#include <QImage>
#include <QPainter>

#include "MarbleGlobal.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "TileLoaderHelper.h"

namespace Marble
{

class TileCreatorPrivate
{
 public:
    TileCreatorPrivate( TileCreatorSource *source,
                        const QString& dem, const QString& targetDir=QString() )
       : m_dem( dem ),
         m_targetDir( targetDir ),
         m_cancelled( false ),
         m_tileFormat( "jpg" ),
         m_resume( false ),
         m_verify( false ),
         m_source( source )
     {
        if (m_dem == QLatin1String("true")) {
            m_tileQuality = 70;
        } else {
            m_tileQuality = 85;
        }
    }

    ~TileCreatorPrivate()
    {
        delete m_source;
    }

 public:
    QString  m_dem;
    QString  m_targetDir;
    bool     m_cancelled;
    QString  m_tileFormat;
    int      m_tileQuality;
    bool     m_resume;
    bool     m_verify;

    TileCreatorSource  *m_source;
};

class TileCreatorSourceImage : public TileCreatorSource
{
public:
    explicit TileCreatorSourceImage( const QString &sourcePath )
        : m_sourceImage( QImage( sourcePath ) ),
          m_cachedRowNum( -1 )
    {
    }

    QSize fullImageSize() const override
    {
        if ( m_sourceImage.size().width() > 21600 || m_sourceImage.height() > 10800 ) {
            qDebug("Install map too large!");
            return QSize();
        }
        return m_sourceImage.size();
    }

    QImage tile(int n, int m, int maxTileLevel) override
    {
        int  mmax = TileLoaderHelper::levelToColumn( defaultLevelZeroColumns, maxTileLevel );
        int  nmax = TileLoaderHelper::levelToRow( defaultLevelZeroRows, maxTileLevel );

        int imageHeight = m_sourceImage.height();
        int imageWidth = m_sourceImage.width();

        // If the image size of the image source does not match the expected
        // geometry we need to smooth-scale the image in advance to match
        // the required size
        bool needsScaling = ( imageWidth != 2 * nmax * (int)( c_defaultTileSize )
                            ||  imageHeight != nmax * (int)( c_defaultTileSize ) );

        if ( needsScaling )
            mDebug() << "Image Size doesn't match 2*n*TILEWIDTH x n*TILEHEIGHT geometry. Scaling ...";

        int  stdImageWidth  = 2 * nmax * c_defaultTileSize;
        if ( stdImageWidth == 0 )
            stdImageWidth = 2 * c_defaultTileSize;

        int  stdImageHeight  = nmax * c_defaultTileSize;
        if ( stdImageWidth != imageWidth ) {
            mDebug() <<
            QString( "TileCreator::createTiles() The size of the final image will measure  %1 x %2 pixels").arg(stdImageWidth).arg(stdImageHeight);
        }

        QImage row;

        if ( m_cachedRowNum == n ) {

            row = m_rowCache;

        } else {

            QRect   sourceRowRect( 0, (int)( (qreal)( n * imageHeight ) / (qreal)( nmax )),
                                imageWidth,(int)( (qreal)( imageHeight ) / (qreal)( nmax ) ) );


            row = m_sourceImage.copy( sourceRowRect );

            if ( needsScaling ) {
                // Pick the current row and smooth scale it
                // to make it match the expected size
                QSize destSize( stdImageWidth, c_defaultTileSize );
                row = row.scaled( destSize,
                                Qt::IgnoreAspectRatio,
                                Qt::SmoothTransformation );
            }

            m_cachedRowNum = n;
            m_rowCache = row;
        }

        if ( row.isNull() ) {
            mDebug() << "Read-Error! Null QImage!";
            return QImage();
        }

        QImage  tile = row.copy( m * stdImageWidth / mmax, 0, c_defaultTileSize, c_defaultTileSize );

        return tile;
    }

private:
    QImage m_sourceImage;

    QImage m_rowCache;
    int m_cachedRowNum;
};


TileCreator::TileCreator(const QString& sourceDir, const QString& installMap,
                         const QString& dem, const QString& targetDir)
    : QThread(nullptr),
      d( new TileCreatorPrivate( nullptr, dem, targetDir ) )

{
    mDebug() << "Prefix: " << sourceDir
        << "installmap:" << installMap;

    QString sourcePath;

    // If the sourceDir starts with a '/' assume an absolute path.
    // Otherwise assume a relative marble data path
    if ( QDir::isAbsolutePath( sourceDir ) ) {
        sourcePath = sourceDir + QLatin1Char('/') + installMap;
        mDebug() << "Trying absolute path*:" << sourcePath;
    }
    else {
        sourcePath = MarbleDirs::path(QLatin1String("maps/") + sourceDir + QLatin1Char('/') + installMap);
        mDebug() << "Trying relative path*:"
                << QLatin1String("maps/") + sourceDir + QLatin1Char('/') + installMap;
    }

    mDebug() << "Creating tiles from*: " << sourcePath;

    d->m_source = new TileCreatorSourceImage( sourcePath );

    if ( d->m_targetDir.isNull() )
        d->m_targetDir = MarbleDirs::localPath() + QLatin1String("/maps/")
            + sourcePath.section(QLatin1Char('/'), -3, -2) + QLatin1Char('/');

    setTerminationEnabled( true );
}

TileCreator::TileCreator( TileCreatorSource* source, const QString& dem, const QString& targetDir )
    : QThread(nullptr),
      d( new TileCreatorPrivate( source, dem, targetDir ) )
{
    setTerminationEnabled( true );
}

TileCreator::~TileCreator()
{
    delete d;
}

void TileCreator::cancelTileCreation()
{
    d->m_cancelled = true;
}

void TileCreator::run()
{
    if (d->m_resume && d->m_tileFormat == QLatin1String("jpg") && d->m_tileQuality != 100) {
        qWarning() << "Resuming jpegs is only supported with tileQuality 100";
        return;
    }

    if (!d->m_targetDir.endsWith(QLatin1Char('/')))
        d->m_targetDir += QLatin1Char('/');

    mDebug() << "Installing tiles to: " << d->m_targetDir;

    QVector<QRgb> grayScalePalette;
    for ( int cnt = 0; cnt <= 255; ++cnt ) {
        grayScalePalette.insert(cnt, qRgb(cnt, cnt, cnt));
    }

    QSize fullImageSize = d->m_source->fullImageSize();
    int  imageWidth  = fullImageSize.width();
    int  imageHeight = fullImageSize.height();

    mDebug() << QString( "TileCreator::createTiles() image dimensions %1 x %2").arg(imageWidth).arg(imageHeight);

    if ( imageWidth < 1 || imageHeight < 1 ) {
        qDebug("Invalid imagemap!");
        return;
    }

    // Calculating Maximum Tile Level
    float approxMaxTileLevel = std::log( imageWidth / ( 2.0 * c_defaultTileSize ) ) / std::log( 2.0 );

    int  maxTileLevel = 0;
    if ( approxMaxTileLevel == int( approxMaxTileLevel ) )
        maxTileLevel = static_cast<int>( approxMaxTileLevel );
    else
        maxTileLevel = static_cast<int>( approxMaxTileLevel + 1 );

    if ( maxTileLevel < 0 ) {
        mDebug() 
        << QString( "TileCreator::createTiles(): Invalid Maximum Tile Level: %1" )
        .arg( maxTileLevel );
    }
    mDebug() << "Maximum Tile Level: " << maxTileLevel;


    if ( !QDir( d->m_targetDir ).exists() )
        ( QDir::root() ).mkpath( d->m_targetDir );

    // Counting total amount of tiles to be generated for the progressbar
    // to prevent compiler warnings this var should
    // match the type of maxTileLevel
    int  tileLevel      = 0;
    int  totalTileCount = 0;

    while ( tileLevel <= maxTileLevel ) {
        totalTileCount += ( TileLoaderHelper::levelToRow( defaultLevelZeroRows, tileLevel )
                            * TileLoaderHelper::levelToColumn( defaultLevelZeroColumns, tileLevel ) );
        tileLevel++;
    }

    mDebug() << totalTileCount << " tiles to be created in total.";

    int  mmax = TileLoaderHelper::levelToColumn( defaultLevelZeroColumns, maxTileLevel );
    int  nmax = TileLoaderHelper::levelToRow( defaultLevelZeroRows, maxTileLevel );

    // Loading each row at highest spatial resolution and cropping tiles
    int      percentCompleted = 0;
    int      createdTilesCount = 0;
    QString  tileName;

    // Creating directory structure for the highest level
    QString  dirName( d->m_targetDir
                      + QString("%1").arg(maxTileLevel) );
    if ( !QDir( dirName ).exists() ) 
        ( QDir::root() ).mkpath( dirName );

    for ( int n = 0; n < nmax; ++n ) {
        QString dirName( d->m_targetDir
                         + QString("%1/%2").arg(maxTileLevel).arg(n, tileDigits, 10, QLatin1Char('0')));
        if ( !QDir( dirName ).exists() ) 
            ( QDir::root() ).mkpath( dirName );
    }

    for ( int n = 0; n < nmax; ++n ) {

        for ( int m = 0; m < mmax; ++m ) {

            mDebug() << "** tile" << m << "x" << n;

            if ( d->m_cancelled ) 
                return;

            tileName = d->m_targetDir + QString("%1/%2/%2_%3.%4")
                                       .arg( maxTileLevel )
                                       .arg(n, tileDigits, 10, QLatin1Char('0'))
                                       .arg(m, tileDigits, 10, QLatin1Char('0'))
                                       .arg( d->m_tileFormat );

            if ( QFile::exists( tileName ) && d->m_resume ) {

                //mDebug() << tileName << "exists already";

            } else {

                QImage tile = d->m_source->tile( n, m, maxTileLevel );

                if ( tile.isNull() ) {
                    mDebug() << "Read-Error! Null QImage!";
                    return;
                }

                if (d->m_dem == QLatin1String("true")) {
                    tile = tile.convertToFormat(QImage::Format_Indexed8,
                                                grayScalePalette,
                                                Qt::ThresholdDither);
                }

                bool  ok = tile.save(tileName, d->m_tileFormat.toLatin1().data(), d->m_tileFormat == QLatin1String("jpg") ? 100 : d->m_tileQuality);
                if ( !ok )
                    mDebug() << "Error while writing Tile: " << tileName;

                mDebug() << tileName << "size" << QFile( tileName ).size();

                if ( d->m_verify ) {
                    QImage writtenTile(tileName);
                    Q_ASSERT( writtenTile.size() == tile.size() );
                    for ( int i=0; i < writtenTile.size().width(); ++i) {
                        for ( int j=0; j < writtenTile.size().height(); ++j) {
                            if ( writtenTile.pixel( i, j ) != tile.pixel( i, j ) ) {
                                unsigned int  pixel = tile.pixel( i, j);
                                unsigned int  writtenPixel = writtenTile.pixel( i, j);
                                qWarning() << "***** pixel" << i << j << "is off by" << (pixel - writtenPixel) << "pixel" << pixel << "writtenPixel" << writtenPixel;
                                QByteArray baPixel((char*)&pixel, sizeof(unsigned int));
                                qWarning() << "pixel" << baPixel.size() << "0x" << baPixel.toHex();
                                QByteArray baWrittenPixel((char*)&writtenPixel, sizeof(unsigned int));
                                qWarning() << "writtenPixel" << baWrittenPixel.size() << "0x" << baWrittenPixel.toHex();
                                Q_ASSERT(false);
                            }
                        }
                    }
                }

            }

            percentCompleted =  (int) ( 90 * (qreal)(createdTilesCount) 
                                        / (qreal)(totalTileCount) );	
            createdTilesCount++;

            mDebug() << "percentCompleted" << percentCompleted;
            emit progress( percentCompleted );
        }
    }

    mDebug() << "tileLevel: " << maxTileLevel << " successfully created.";

    tileLevel = maxTileLevel;

    // Now that we have the tiles at the highest resolution lets build
    // them together four by four.

    while( tileLevel > 0 ) {
        tileLevel--;

        int  nmaxit =  TileLoaderHelper::levelToRow( defaultLevelZeroRows, tileLevel );

        for ( int n = 0; n < nmaxit; ++n ) {
            QString  dirName( d->m_targetDir
                              + QString("%1/%2")
                                  .arg(tileLevel)
                                  .arg(n, tileDigits, 10, QLatin1Char('0')));

            // mDebug() << "dirName: " << dirName;
            if ( !QDir( dirName ).exists() ) 
                ( QDir::root() ).mkpath( dirName );

            int   mmaxit = TileLoaderHelper::levelToColumn( defaultLevelZeroColumns, tileLevel );
            for ( int m = 0; m < mmaxit; ++m ) {

                if ( d->m_cancelled )
                    return;

                QString newTileName = d->m_targetDir + QString("%1/%2/%2_%3.%4")
                                           .arg( tileLevel )
                                           .arg(n, tileDigits, 10, QLatin1Char('0'))
                                           .arg(m, tileDigits, 10, QLatin1Char('0'))
                                           .arg( d->m_tileFormat );

                if ( QFile::exists( newTileName ) && d->m_resume ) {
                    //mDebug() << newTileName << "exists already";
                } else {
                    tileName = d->m_targetDir + QString("%1/%2/%2_%3.%4")
                                            .arg( tileLevel + 1 )
                                            .arg(2*n, tileDigits, 10, QLatin1Char('0'))
                                            .arg(2*m, tileDigits, 10, QLatin1Char('0'))
                                            .arg( d->m_tileFormat );
                    QImage  img_topleft( tileName );

                    tileName = d->m_targetDir + QString("%1/%2/%2_%3.%4")
                                            .arg( tileLevel + 1 )
                                            .arg(2*n, tileDigits, 10, QLatin1Char('0'))
                                            .arg(2*m+1, tileDigits, 10, QLatin1Char('0'))
                                            .arg( d->m_tileFormat );
                    QImage  img_topright( tileName );

                    tileName = d->m_targetDir + QString("%1/%2/%2_%3.%4")
                                            .arg( tileLevel + 1 )
                                            .arg(2*n+1, tileDigits, 10, QLatin1Char('0'))
                                            .arg(2*m, tileDigits, 10, QLatin1Char('0'))
                                            .arg( d->m_tileFormat );
                    QImage  img_bottomleft( tileName );

                    tileName = d->m_targetDir + QString("%1/%2/%2_%3.%4")
                                            .arg( tileLevel + 1 )
                                            .arg(2*n+1, tileDigits, 10, QLatin1Char('0'))
                                            .arg(2*m+1, tileDigits, 10, QLatin1Char('0'))
                                            .arg( d->m_tileFormat );
                    QImage  img_bottomright( tileName );

                    QSize const expectedSize( c_defaultTileSize, c_defaultTileSize );
                    if ( img_topleft.size() != expectedSize ||
                         img_topright.size() != expectedSize ||
                         img_bottomleft.size() != expectedSize ||
                         img_bottomright.size() != expectedSize ) {
                        mDebug() << "Tile write failure. Missing write permissions?";
                        emit progress( 100 );
                        return;
                    }
                    QImage  tile = img_topleft;

                    if (d->m_dem == QLatin1String("true")) {

                        tile.setColorTable( grayScalePalette );
                        uchar* destLine;

                        for ( uint y = 0; y < c_defaultTileSize / 2; ++y ) {
                            destLine = tile.scanLine( y );
                            const uchar* srcLine = img_topleft.scanLine( 2 * y );
                            for ( uint x = 0; x < c_defaultTileSize / 2; ++x )
                                destLine[x] = srcLine[ 2*x ];
                        }
                        for ( uint y = 0; y < c_defaultTileSize / 2; ++y ) {
                            destLine = tile.scanLine( y );
                            const uchar* srcLine = img_topright.scanLine( 2 * y );
                            for ( uint x = c_defaultTileSize / 2; x < c_defaultTileSize; ++x )
                                destLine[x] = srcLine[ 2 * ( x - c_defaultTileSize / 2 ) ];
                        }
                        for ( uint y = c_defaultTileSize / 2; y < c_defaultTileSize; ++y ) {
                            destLine = tile.scanLine( y );
                            const uchar* srcLine = img_bottomleft.scanLine( 2 * ( y - c_defaultTileSize / 2 ) );
                            for ( uint x = 0; x < c_defaultTileSize / 2; ++x )
                                destLine[ x ] = srcLine[ 2 * x ];
                        }
                        for ( uint y = c_defaultTileSize / 2; y < c_defaultTileSize; ++y ) {
                            destLine = tile.scanLine( y );
                            const uchar* srcLine = img_bottomright.scanLine( 2 * ( y - c_defaultTileSize/2 ) );
                            for ( uint x = c_defaultTileSize / 2; x < c_defaultTileSize; ++x )
                                destLine[x] = srcLine[ 2 * ( x - c_defaultTileSize / 2 ) ];
                        }
                    }
                    else {

                        // tile.depth() != 8

                        img_topleft = img_topleft.convertToFormat( QImage::Format_ARGB32 );
                        img_topright = img_topright.convertToFormat( QImage::Format_ARGB32 );
                        img_bottomleft = img_bottomleft.convertToFormat( QImage::Format_ARGB32 );
                        img_bottomright = img_bottomright.convertToFormat( QImage::Format_ARGB32 );
                        tile = img_topleft;

                        QRgb* destLine;

                        for ( uint y = 0; y < c_defaultTileSize / 2; ++y ) {
                            destLine = (QRgb*) tile.scanLine( y );
                            const QRgb* srcLine = (QRgb*) img_topleft.scanLine( 2 * y );
                            for ( uint x = 0; x < c_defaultTileSize / 2; ++x )
                                destLine[x] = srcLine[ 2 * x ];
                        }
                        for ( uint y = 0; y < c_defaultTileSize / 2; ++y ) {
                            destLine = (QRgb*) tile.scanLine( y );
                            const QRgb* srcLine = (QRgb*) img_topright.scanLine( 2 * y );
                            for ( uint x = c_defaultTileSize / 2; x < c_defaultTileSize; ++x )
                                destLine[x] = srcLine[ 2 * ( x - c_defaultTileSize / 2 ) ];
                        }
                        for ( uint y = c_defaultTileSize / 2; y < c_defaultTileSize; ++y ) {
                            destLine = (QRgb*) tile.scanLine( y );
                            const QRgb* srcLine = (QRgb*) img_bottomleft.scanLine( 2 * ( y-c_defaultTileSize/2 ) );
                            for ( uint x = 0; x < c_defaultTileSize / 2; ++x )
                                destLine[x] = srcLine[ 2 * x ];
                        }
                        for ( uint y = c_defaultTileSize / 2; y < c_defaultTileSize; ++y ) {
                            destLine = (QRgb*) tile.scanLine( y );
                            const QRgb* srcLine = (QRgb*) img_bottomright.scanLine( 2 * ( y-c_defaultTileSize / 2 ) );
                            for ( uint x = c_defaultTileSize / 2; x < c_defaultTileSize; ++x )
                                destLine[x] = srcLine[ 2*( x-c_defaultTileSize / 2 ) ];
                        }
                    }

                    mDebug() << newTileName;

                    // Saving at 100% JPEG quality to have a high-quality
                    // version to create the remaining needed tiles from.
                    bool  ok = tile.save(newTileName, d->m_tileFormat.toLatin1().data(), d->m_tileFormat == QLatin1String("jpg") ? 100 : d->m_tileQuality);
                    if ( ! ok )
                        mDebug() << "Error while writing Tile: " << newTileName;
                }

                percentCompleted =  (int) ( 90 * (qreal)(createdTilesCount)
                                            / (qreal)(totalTileCount) );	
                createdTilesCount++;
						
                emit progress( percentCompleted );
                mDebug() << "percentCompleted" << percentCompleted;
            }
        }
        mDebug() << "tileLevel: " << tileLevel << " successfully created.";
    }
    mDebug() << "Tile creation completed.";

    if (d->m_tileFormat == QLatin1String("jpg") && d->m_tileQuality != 100) {

        // Applying correct lower JPEG compression now that we created all tiles
        int savedTilesCount = 0;

        tileLevel = 0;
        while ( tileLevel <= maxTileLevel ) {
            int nmaxit =  TileLoaderHelper::levelToRow( defaultLevelZeroRows, tileLevel );
            for ( int n = 0; n < nmaxit; ++n) {
                int mmaxit =  TileLoaderHelper::levelToColumn( defaultLevelZeroColumns, tileLevel );
                for ( int m = 0; m < mmaxit; ++m) {

                    if ( d->m_cancelled )
                        return;

                    savedTilesCount++;

                    tileName = d->m_targetDir + QString("%1/%2/%2_%3.%4")
                                            .arg( tileLevel )
                                            .arg(n, tileDigits, 10, QLatin1Char('0'))
                                            .arg(m, tileDigits, 10, QLatin1Char('0'))
                                            .arg( d->m_tileFormat );
                    QImage tile( tileName );

                    bool ok;

                    ok = tile.save( tileName, d->m_tileFormat.toLatin1().data(), d->m_tileQuality );

                    if ( !ok )
                        mDebug() << "Error while writing Tile: " << tileName;
                    // Don't exceed 99% as this would cancel the thread unexpectedly
                    percentCompleted = 90 + (int)( 9 * (qreal)(savedTilesCount)
                                                / (qreal)(totalTileCount) );
                    emit progress( percentCompleted );
                    mDebug() << "percentCompleted" << percentCompleted;
                    //mDebug() << "Saving Tile #" << savedTilesCount
                    //         << " of " << totalTileCount
                    //         << " Percent: " << percentCompleted;
                }
            }
            tileLevel++;
        }
    }

    percentCompleted = 100;
    emit progress( percentCompleted );

    mDebug() << "percentCompleted: " << percentCompleted;
}

void TileCreator::setTileFormat(const QString& format)
{
    d->m_tileFormat = format;
}

QString TileCreator::tileFormat() const
{
    return d->m_tileFormat;
}

void TileCreator::setTileQuality(int quality)
{
    d->m_tileQuality = quality;
}

int TileCreator::tileQuality() const
{
    return d->m_tileQuality;
}

void TileCreator::setResume(bool resume)
{
    d->m_resume = resume;
}

bool TileCreator::resume() const
{
    return d->m_resume;
}

void TileCreator::setVerifyExactResult(bool verify)
{
    d->m_verify = verify;
}

bool TileCreator::verifyExactResult() const
{
    return d->m_verify;
}


}

#include "moc_TileCreator.cpp"
