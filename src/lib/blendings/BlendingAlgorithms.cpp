// Copyright 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library. If not, see <http://www.gnu.org/licenses/>.

#include "BlendingAlgorithms.h"

#include "TextureTile.h"

#include <cmath>

#include <QtGui/QImage>

namespace Marble
{

// pre-conditions:
// - bottom and top image have the same size
// - bottom image format is ARGB32_Premultiplied
void IndependentChannelBlending::blend( QImage * const bottom,
                                        QSharedPointer<TextureTile> const & top ) const
{
    QImage const * const topImage = top->image();
    Q_ASSERT( topImage );
    Q_ASSERT( bottom->size() == topImage->size() );
    Q_ASSERT( bottom->format() == QImage::Format_ARGB32_Premultiplied );

    int const width = bottom->width();
    int const height = bottom->height();
    QImage const topImagePremult = topImage->convertToFormat( QImage::Format_ARGB32_Premultiplied );
    for ( int y = 0; y < height; ++y ) {
        for ( int x = 0; x < width; ++x ) {
            QRgb const bottomPixel = bottom->pixel( x, y );
            QRgb const topPixel = topImagePremult.pixel( x, y );
            qreal const resultRed = blendChannel( qRed( bottomPixel ) / 255.0,
                                                  qRed( topPixel ) / 255.0 );
            qreal const resultGreen = blendChannel( qGreen( bottomPixel ) / 255.0,
                                                    qGreen( topPixel ) / 255.0 );
            qreal const resultBlue = blendChannel( qBlue( bottomPixel ) / 255.0,
                                                   qBlue( topPixel ) / 255.0 );
            bottom->setPixel( x, y, qRgb( resultRed * 255.0,
                                          resultGreen * 255.0,
                                          resultBlue * 255.0 ));
        }
    }
}


// Neutral blendings

qreal AllanonBlending::blendChannel( qreal const bottomColorIntensity,
                                     qreal const topColorIntensity ) const
{
    return ( bottomColorIntensity + topColorIntensity ) / 2.0;
}

qreal ArcusTangentBlending::blendChannel( qreal const bottomColorIntensity,
                                          qreal const topColorIntensity ) const
{
    return 2.0 * atan( topColorIntensity / bottomColorIntensity ) / M_PI;
}

qreal GeometricMeanBlending::blendChannel( qreal const bottomColorIntensity,
                                           qreal const topColorIntensity ) const
{
    return sqrt( bottomColorIntensity * topColorIntensity );
}

qreal LinearLightBlending::blendChannel( qreal const bottomColorIntensity,
                                         qreal const topColorIntensity ) const
{
    return qMin( qreal( 1.0 ),
                 qMax( qreal( 0.0 ), ( bottomColorIntensity + 2.0 * topColorIntensity ) - 1.0 ));
}

qreal OverlayBlending::blendChannel( qreal const bottomColorIntensity,
                                     qreal const topColorIntensity ) const
{
    if ( bottomColorIntensity < 0.5 )
        return 2.0 * bottomColorIntensity * topColorIntensity;
    else
        return 1.0 - 2.0 * ( 1.0 - bottomColorIntensity ) * ( 1.0 - topColorIntensity );
}

qreal ParallelBlending::blendChannel( qreal const bottomColorIntensity,
                                      qreal const topColorIntensity ) const
{
    Q_UNUSED(bottomColorIntensity);
    Q_UNUSED(topColorIntensity);
    // FIXME:    return qMin( qMax( 2.0 / ( 1.0 / bottomColorIntensity + 1.0 / topColorIntensity )), 0.0, 1.0 );
    return 0.0;
}

qreal TextureBlending::blendChannel( qreal const bottomColorIntensity,
                                     qreal const topColorIntensity ) const
{
    Q_UNUSED(bottomColorIntensity);
    Q_UNUSED(topColorIntensity);
    // FIXME: return qMax( qMin( topColorIntensity + bottomColorIntensity ) - 0.5 ), 1.0 ), 0.0 );
    return 0.0;
}


// Darkening blendings

qreal ColorBurnBlending::blendChannel( qreal const bottomColorIntensity,
                                       qreal const topColorIntensity ) const
{
    Q_UNUSED(bottomColorIntensity);
    Q_UNUSED(topColorIntensity);
    // FIXME: check if this formula makes sense
    return qMin( qreal( 1.0 ),
                 qMax( qreal( 0.0 ), 1.0 - ( 1.0 - bottomColorIntensity ) / topColorIntensity ));
}

qreal DarkBlending::blendChannel( qreal const bottomColorIntensity,
                                  qreal const topColorIntensity ) const
{
    return ( bottomColorIntensity + 1.0 - topColorIntensity ) * topColorIntensity;
}

qreal DarkenBlending::blendChannel( qreal const bottomColorIntensity,
                                    qreal const topColorIntensity ) const
{
    // FIXME: is this really ok? not vice versa?
    return bottomColorIntensity > topColorIntensity ? topColorIntensity : bottomColorIntensity;
}

qreal DivideBlending::blendChannel( qreal const bottomColorIntensity,
                                    qreal const topColorIntensity ) const
{
    return log( 1.0  + bottomColorIntensity / ( 1.0  - topColorIntensity ) / 8.0) / log(2.0);
}

qreal GammaDarkBlending::blendChannel( qreal const bottomColorIntensity,
                                       qreal const topColorIntensity ) const
{
    return pow( bottomColorIntensity, 1.0 / topColorIntensity );
}

qreal LinearBurnBlending::blendChannel( qreal const bottomColorIntensity,
                                        qreal const topColorIntensity ) const
{
    return qMax( 0.0, bottomColorIntensity + topColorIntensity - 1.0 );
}

qreal MultiplyBlending::blendChannel( qreal const bottomColorIntensity,
                                      qreal const topColorIntensity ) const
{
    return bottomColorIntensity * topColorIntensity;
}

qreal SubtractiveBlending::blendChannel( qreal const bottomColorIntensity,
                                         qreal const topColorIntensity ) const
{
    return qMax( bottomColorIntensity - topColorIntensity, qreal(0.0) );
}


// Lightening blendings

void AlphaBlending::blend( QImage * const bottom, QSharedPointer< TextureTile > const & top ) const
{
    QImage const * const topImage = top->image();
    Q_ASSERT( topImage );
    Q_ASSERT( bottom->size() == topImage->size() );
    int const width = bottom->width();
    int const height = bottom->height();
    for ( int y = 0; y < height; ++y ) {
        for ( int x = 0; x < width; ++x ) {
            qreal const c = qRed( topImage->pixel( x, y )) / 255.0;
            qreal const alpha = qAlpha(topImage->pixel( x, y )) / 255.0;
            QRgb const bottomPixel = bottom->pixel( x, y );
            int const bottomRed = qRed( bottomPixel );
            int const bottomGreen = qGreen( bottomPixel );
            int const bottomBlue = qBlue( bottomPixel );
            bottom->setPixel( x, y, qRgb(( int )( c * alpha + ( 1 - alpha ) * bottomRed ),
                                         ( int )( c * alpha + ( 1 - alpha ) * bottomGreen ),
                                         ( int )( c * alpha + ( 1 - alpha ) * bottomBlue )));
        }
    }
}

qreal AdditiveBlending::blendChannel( qreal const bottomColorIntensity,
                                      qreal const topColorIntensity ) const
{
    return qMin( topColorIntensity + bottomColorIntensity, qreal(1.0) );
}

qreal ColorDodgeBlending::blendChannel( qreal const bottomColorIntensity,
                                        qreal const topColorIntensity ) const
{
    return qMin( qreal( 1.0 ),
                 qMax( qreal( 0.0 ), bottomColorIntensity / ( 1.0 - topColorIntensity )));
}

qreal GammaLightBlending::blendChannel( qreal const bottomColorIntensity,
                                        qreal const topColorIntensity ) const
{
    return pow( bottomColorIntensity, topColorIntensity );
}

qreal HardLightBlending::blendChannel( qreal const bottomColorIntensity,
                                       qreal const topColorIntensity ) const
{
    return topColorIntensity < 0.5
        ? 2.0 * bottomColorIntensity * topColorIntensity
        : 1.0 - 2.0 * ( 1.0 - bottomColorIntensity ) * ( 1.0 - topColorIntensity );
}

qreal LightBlending::blendChannel( qreal const bottomColorIntensity,
                                   qreal const topColorIntensity ) const
{
    return bottomColorIntensity * ( 1.0 - topColorIntensity ) + pow( topColorIntensity, 2 );
}

qreal LightenBlending::blendChannel( qreal const bottomColorIntensity,
                                     qreal const topColorIntensity ) const
{
    // is this ok?
    return bottomColorIntensity < topColorIntensity ? topColorIntensity : bottomColorIntensity;
}

qreal PinLightBlending::blendChannel( qreal const bottomColorIntensity,
                                      qreal const topColorIntensity ) const
{
    return qMax( qreal(0.0), qMax( qreal(2.0 + topColorIntensity - 1.0),
                             qMin( bottomColorIntensity, qreal(2.0 * topColorIntensity ))));
}

qreal ScreenBlending::blendChannel( qreal const bottomColorIntensity,
                                    qreal const topColorIntensity ) const
{
    return 1.0 - ( 1.0 - bottomColorIntensity ) * ( 1.0 - topColorIntensity );
}

qreal SoftLightBlending::blendChannel( qreal const bottomColorIntensity,
                                       qreal const topColorIntensity ) const
{
    return pow( bottomColorIntensity, pow( 2.0, ( 2.0 * ( 0.5 - topColorIntensity ))));
}

qreal VividLightBlending::blendChannel( qreal const bottomColorIntensity,
                                        qreal const topColorIntensity ) const
{
    return topColorIntensity < 0.5
        ? qMin( qreal( 1.0 ),
                qMax( qreal( 0.0 ), 1.0 - ( 1.0 - bottomColorIntensity ) / ( 2.0 * topColorIntensity )))
        : qMin( qreal( 1.0 ),
                qMax( qreal( 0.0 ), bottomColorIntensity / ( 2.0 * ( 1.0 - topColorIntensity ))));
}


// Inverter blendings

qreal AdditiveSubtractiveBlending::blendChannel( qreal const bottomColorIntensity,
                                                 qreal const topColorIntensity ) const
{
    Q_UNUSED(bottomColorIntensity);
    Q_UNUSED(topColorIntensity);
    // FIXME:
    //    return qMin( 1.0, qMax( 0.0, abs( bottomColorIntensity * bottomColorIntensity
    //                                      - topColorIntensity * topColorIntensity )));
    return 0.0;
}

qreal BleachBlending::blendChannel( qreal const bottomColorIntensity,
                                    qreal const topColorIntensity ) const
{
    // FIXME: "why this is the same formula as Screen Blending? Please correct.)"
    return 1.0 - ( 1.0 - bottomColorIntensity ) * ( 1.0 - topColorIntensity );
}

qreal DifferenceBlending::blendChannel( qreal const bottomColorIntensity,
                                        qreal const topColorIntensity ) const
{
    return qMax( qMin( qreal( 1.0 ), bottomColorIntensity - topColorIntensity + 0.5 ),
                 qreal( 0.0 ));
}

qreal EquivalenceBlending::blendChannel( qreal const bottomColorIntensity,
                                         qreal const topColorIntensity ) const
{
    return 1.0 - abs( bottomColorIntensity - topColorIntensity );
}

qreal HalfDifferenceBlending::blendChannel( qreal const bottomColorIntensity,
                                            qreal const topColorIntensity ) const
{
    return bottomColorIntensity + topColorIntensity
        - 2.0 * ( bottomColorIntensity * topColorIntensity );
}


// Special purpose blendings

void CloudsBlending::blend( QImage * const bottom, QSharedPointer<TextureTile> const & top ) const
{
    QImage const * const topImage = top->image();
    Q_ASSERT( topImage );
    Q_ASSERT( bottom->size() == topImage->size() );
    int const width = bottom->width();
    int const height = bottom->height();
    for ( int y = 0; y < height; ++y ) {
        for ( int x = 0; x < width; ++x ) {
            qreal const c = qRed( topImage->pixel( x, y )) / 255.0;
            QRgb const bottomPixel = bottom->pixel( x, y );
            int const bottomRed = qRed( bottomPixel );
            int const bottomGreen = qGreen( bottomPixel );
            int const bottomBlue = qBlue( bottomPixel );
            bottom->setPixel( x, y, qRgb(( int )( bottomRed + ( 255 - bottomRed ) * c ),
                                         ( int )( bottomGreen + ( 255 - bottomGreen ) * c ),
                                         ( int )( bottomBlue + ( 255 - bottomBlue ) * c )));
        }
    }
}


}
