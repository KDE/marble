//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2010 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008-2010 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "StackedTile.h"
#include "StackedTile_p.h"

#include <QtGui/QImage>

#include "MarbleDebug.h"
#include "TextureTile.h"

using namespace Marble;

static const uint **jumpTableFromQImage32( const QImage &img )
{
    if ( img.depth() != 48 && img.depth() != 32 )
        return 0;

    const int  height = img.height();
    const int  bpl    = img.bytesPerLine() / 4;
    const uint  *data = reinterpret_cast<const QRgb*>(img.bits());
    const uint **jumpTable = new const uint*[height];

    for ( int y = 0; y < height; ++y ) {
        jumpTable[ y ] = data;
        data += bpl;
    }

    return jumpTable;
}


static const uchar **jumpTableFromQImage8( const QImage &img )
{
    if ( img.depth() != 8 && img.depth() != 1 )
        return 0;

    const int  height = img.height();
    const int  bpl    = img.bytesPerLine();
    const uchar  *data = img.bits();
    const uchar **jumpTable = new const uchar*[height];

    for ( int y = 0; y < height; ++y ) {
        jumpTable[ y ] = data;
        data += bpl;
    }

    return jumpTable;
}


StackedTilePrivate::StackedTilePrivate( const TileId &id, const QImage &resultImage, QVector<QSharedPointer<TextureTile> > const &tiles ) :
      m_id( id ), 
      m_resultTile( resultImage ),
      m_depth( resultImage.depth() ),
      m_isGrayscale( resultImage.isGrayscale() ),
      m_tiles( tiles ),
      jumpTable8( jumpTableFromQImage8( m_resultTile ) ),
      jumpTable32( jumpTableFromQImage32( m_resultTile ) ),
      m_byteCount( calcByteCount( resultImage, tiles ) )
{
}

StackedTilePrivate::~StackedTilePrivate()
{
      delete [] jumpTable32;
      delete [] jumpTable8;
}

uint StackedTilePrivate::pixel( int x, int y ) const
{
    if ( m_depth == 8 ) {
        if ( m_isGrayscale )
            return (jumpTable8)[y][x];
        else
            return m_resultTile.color( (jumpTable8)[y][x] );
    }
    if ( m_depth == 32 )
        return (jumpTable32)[y][x];

    if ( m_depth == 1 && !m_isGrayscale )
        return m_resultTile.color((jumpTable8)[y][x/8] >> 7);

    return m_resultTile.pixel( x, y );
}

uint StackedTilePrivate::pixelF( qreal x, qreal y, const QRgb& topLeftValue ) const
{
    // Bilinear interpolation to determine the color of a subpixel 

    int iX = (int)(x);
    int iY = (int)(y);

    qreal fY = y - iY;

    // Interpolation in y-direction
    if ( ( iY + 1 ) < m_resultTile.height() ) {

        QRgb bottomLeftValue  =  pixel( iX, iY + 1 );
// #define CHEAPHIGH
#ifdef CHEAPHIGH
        QRgb leftValue;
        if ( fY < 0.33 )
            leftValue = topLeftValue;
        else if ( fY < 0.66 ) 
            leftValue = (((bottomLeftValue ^ topLeftValue) & 0xfefefefeUL) >> 1)
                            + (bottomLeftValue & topLeftValue);
        else 
            leftValue = bottomLeftValue;
#else
        // blending the color values of the top left and bottom left point
        qreal ml_red   = ( 1.0 - fY ) * qRed  ( topLeftValue  ) + fY * qRed  ( bottomLeftValue  );
        qreal ml_green = ( 1.0 - fY ) * qGreen( topLeftValue  ) + fY * qGreen( bottomLeftValue  );
        qreal ml_blue  = ( 1.0 - fY ) * qBlue ( topLeftValue  ) + fY * qBlue ( bottomLeftValue  );
#endif
        // Interpolation in x-direction
        if ( iX + 1 < m_resultTile.width() ) {

            qreal fX = x - iX;

            QRgb topRightValue    =  pixel( iX + 1, iY );
            QRgb bottomRightValue =  pixel( iX + 1, iY + 1 );

#ifdef CHEAPHIGH
            QRgb rightValue;
            if ( fY < 0.33 )
                rightValue = topRightValue;
            else if ( fY < 0.66 )
                rightValue = (((bottomRightValue ^ topRightValue) & 0xfefefefeUL) >> 1)
                                + (bottomRightValue & topRightValue);
            else
                rightValue = bottomRightValue;

            QRgb averageValue;
            if ( fX < 0.33 )
                averageValue = leftValue;
            else if ( fX < 0.66 )
                averageValue = (((leftValue ^ rightValue) & 0xfefefefeUL) >> 1)
                                + (leftValue & rightValue);
            else
                averageValue = rightValue;

            return averageValue;
#else
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
#endif
        }
        else {
#ifdef CHEAPHIGH
            return leftValue;
#else
            return qRgb( ml_red, ml_green, ml_blue );
#endif
        }
    }
    else {
        // Interpolation in x-direction
        if ( iX + 1 < m_resultTile.width() ) {

            qreal fX = x - iX;

            if ( fX == 0.0 ) 
                return topLeftValue;

            QRgb topRightValue    =  pixel( iX + 1, iY );
#ifdef CHEAPHIGH
            QRgb topValue;
            if ( fX < 0.33 )
                topValue = topLeftValue;
            else if ( fX < 0.66 )
                topValue = (((topLeftValue ^ topRightValue) & 0xfefefefeUL) >> 1)
                                + (topLeftValue & topRightValue);
            else
                topValue = topRightValue;

            return topValue;
#else
            // blending the color values of the top left and top right point
            int tm_red   = (int)( ( 1.0 - fX ) * qRed  ( topLeftValue ) + fX * qRed  ( topRightValue ) );
            int tm_green = (int)( ( 1.0 - fX ) * qGreen( topLeftValue ) + fX * qGreen( topRightValue ) );
            int tm_blue  = (int)( ( 1.0 - fX ) * qBlue ( topLeftValue ) + fX * qBlue ( topRightValue ) );

            return qRgb( tm_red, tm_green, tm_blue );
#endif
        }
    }

    return topLeftValue;
}

int StackedTilePrivate::calcByteCount( const QImage &resultImage, const QVector<QSharedPointer<TextureTile> > &tiles )
{
    int byteCount = resultImage.numBytes();

    QVector<QSharedPointer<TextureTile> >::const_iterator pos = tiles.constBegin();
    QVector<QSharedPointer<TextureTile> >::const_iterator const end = tiles.constEnd();
    for (; pos != end; ++pos )
        byteCount += (*pos)->byteCount();

    return byteCount;
}


StackedTile::StackedTile( TileId const &id, QImage const &resultImage, QVector<QSharedPointer<TextureTile> > const &tiles )
    : d( new StackedTilePrivate( id, resultImage, tiles ) )
{
    Q_ASSERT( !tiles.isEmpty() );

    if ( d->m_resultTile.isNull() ) {
        qWarning() << "An essential tile is missing. Please rerun the application.";
        return;
    }

    if ( d->jumpTable32 == 0 && d->jumpTable8 == 0 ) {
        qWarning() << "Color depth" << d->m_depth << " is not supported.";
    }
}

StackedTile::~StackedTile()
{
    delete d;
}

TileId const& StackedTile::id() const
{
    return d->m_id;
}

uint StackedTile::pixel( int x, int y ) const
{
    return d->pixel( x, y );
}

uint StackedTile::pixelF( qreal x, qreal y ) const
{
    int iX = (int)(x);
    int iY = (int)(y);

    QRgb topLeftValue  =  pixel( iX, iY );

    return d->pixelF( x, y, topLeftValue );
}

uint StackedTile::pixelF( qreal x, qreal y, const QRgb& topLeftValue ) const
{
    return d->pixelF( x, y, topLeftValue );
}

int StackedTile::depth() const
{
    return d->m_depth;
}

int StackedTile::numBytes() const
{
    return d->m_byteCount;
}

QVector<QSharedPointer<TextureTile> > StackedTile::tiles() const
{
    return d->m_tiles;
}

QImage const * StackedTile::resultTile() const
{
    return &d->m_resultTile;
}
