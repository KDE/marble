//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013        Adrian Draghici <draghici.adrian.b@gmail.com>

#include "ImageF.h"

namespace Marble {

ImageF::ImageF()
{
}

uint ImageF::pixelF( const QImage& image, qreal x, qreal y )
{

    const QRgb& topLeftPixel = image.pixel( (int) x, (int) y );

    // Bilinear interpolation to determine the color of a subpixel

    int iX = int( x );
    int iY = int( y );

    qreal fY = y - iY;

    // Interpolation in y-direction
    if ( ( iY + 1 ) < image.height() ) {

        QRgb bottomLeftPixel  =  image.pixel( iX, iY + 1 );

        // Blending the color values of the top left and bottom left point
        qreal ml_red   = ( 1.0 - fY ) * qRed  ( topLeftPixel  ) + fY * qRed  ( bottomLeftPixel  );
        qreal ml_green = ( 1.0 - fY ) * qGreen( topLeftPixel  ) + fY * qGreen( bottomLeftPixel  );
        qreal ml_blue  = ( 1.0 - fY ) * qBlue ( topLeftPixel  ) + fY * qBlue ( bottomLeftPixel  );

        // Interpolation in x-direction
        if ( iX + 1 < image.width() ) {

            qreal fX = x - iX;

            QRgb topRightPixel    =  image.pixel( iX + 1, iY );
            QRgb bottomRightPixel =  image.pixel( iX + 1, iY + 1 );

            // Blending the color values of the top right and bottom right point
            qreal mr_red   = ( 1.0 - fY ) * qRed  ( topRightPixel ) + fY * qRed  ( bottomRightPixel );
            qreal mr_green = ( 1.0 - fY ) * qGreen( topRightPixel ) + fY * qGreen( bottomRightPixel );
            qreal mr_blue  = ( 1.0 - fY ) * qBlue ( topRightPixel ) + fY * qBlue ( bottomRightPixel );

            // Blending the color values of the resulting middle left and middle right points
            int mm_red   = int( ( ( 1.0 - fX ) * ml_red   + fX * mr_red   ) );
            int mm_green = int( ( ( 1.0 - fX ) * ml_green + fX * mr_green ) );
            int mm_blue  = int( ( ( 1.0 - fX ) * ml_blue  + fX * mr_blue  ) );

            return qRgb( mm_red, mm_green, mm_blue );
        }
        else {
            return qRgb( ml_red, ml_green, ml_blue );
        }
    }
    else {
        // Interpolation in x-direction
        if ( iX + 1 < image.width() ) {

            qreal fX = x - iX;

            if ( fX == 0.0 )
                return topLeftPixel;

            QRgb topRightPixel    =  image.pixel( iX + 1, iY );

            // Blending the color values of the top left and top right point
            int tm_red   = int( ( ( 1.0 - fX ) * qRed  ( topLeftPixel ) + fX * qRed  ( topRightPixel ) ) );
            int tm_green = int( ( ( 1.0 - fX ) * qGreen( topLeftPixel ) + fX * qGreen( topRightPixel ) ) );
            int tm_blue  = int( ( ( 1.0 - fX ) * qBlue ( topLeftPixel ) + fX * qBlue ( topRightPixel ) ) );

            return qRgb( tm_red, tm_green, tm_blue );
        }
    }

    return topLeftPixel;
}

}
