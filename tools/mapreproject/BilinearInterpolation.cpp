#include "BilinearInterpolation.h"

#include "ReadOnlyMapImage.h"

#include <cmath>

BilinearInterpolation::BilinearInterpolation( ReadOnlyMapImage * const mapImage )
    : m_mapImage( mapImage )
{
}

QRgb BilinearInterpolation::interpolate( double const x, double const y )
{
    int const x1 = x;
    int const x2 = x1 + 1;
    int const y1 = y;
    int const y2 = y1 + 1;

    QRgb const lowerLeftPixel = m_mapImage->pixel( x1, y1 );
    QRgb const lowerRightPixel = m_mapImage->pixel( x2, y1 );
    QRgb const upperLeftPixel = m_mapImage->pixel( x1, y2 );
    QRgb const upperRightPixel = m_mapImage->pixel( x2, y2 );

    // interpolate horizontically
    //
    // x2 - x    x2 - x
    // ------- = ------ = x1 + 1 - x = 1 - fractionX
    // x2 - x1      1
    //
    // x - x1    x - x1
    // ------- = ------ = fractionX
    // x2 - x1     1

    double const fractionX = x - x1;
    double const lowerMidRed   = ( 1.0 - fractionX ) * qRed( lowerLeftPixel )   + fractionX * qRed( lowerRightPixel );
    double const lowerMidGreen = ( 1.0 - fractionX ) * qGreen( lowerLeftPixel ) + fractionX * qGreen( lowerRightPixel );
    double const lowerMidBlue  = ( 1.0 - fractionX ) * qBlue( lowerLeftPixel )  + fractionX * qBlue( lowerRightPixel );
    double const lowerMidAlpha = ( 1.0 - fractionX ) * qAlpha( lowerLeftPixel ) + fractionX * qAlpha( lowerRightPixel );

    double const upperMidRed   = ( 1.0 - fractionX ) * qRed( upperLeftPixel )   + fractionX * qRed( upperRightPixel );
    double const upperMidGreen = ( 1.0 - fractionX ) * qGreen( upperLeftPixel ) + fractionX * qGreen( upperRightPixel );
    double const upperMidBlue  = ( 1.0 - fractionX ) * qBlue( upperLeftPixel )  + fractionX * qBlue( upperRightPixel );
    double const upperMidAlpha = ( 1.0 - fractionX ) * qAlpha( upperLeftPixel ) + fractionX * qAlpha( upperRightPixel );

    // interpolate vertically
    //
    // y2 - y    y2 - y
    // ------- = ------ = y1 + 1 - y = 1 - fractionY
    // y2 - y1      1
    //
    // y - y1    y - y1
    // ------- = ------ = fractionY
    // y2 - y1     1

    double const fractionY = y - y1;
    double const red   = ( 1.0 - fractionY ) * lowerMidRed   + fractionY * upperMidRed;
    double const green = ( 1.0 - fractionY ) * lowerMidGreen + fractionY * upperMidGreen;
    double const blue  = ( 1.0 - fractionY ) * lowerMidBlue  + fractionY * upperMidBlue;
    double const alpha = ( 1.0 - fractionY ) * lowerMidAlpha + fractionY * upperMidAlpha;

    return qRgba( round( red ), round( green ), round( blue ), round( alpha ));
}

void BilinearInterpolation::setMapImage( ReadOnlyMapImage * const mapImage )
{
    m_mapImage = mapImage;
}
