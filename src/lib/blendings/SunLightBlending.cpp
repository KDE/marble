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

#include "SunLightBlending.h"

#include "MarbleClock.h"
#include "MarbleDebug.h"
#include "Planet.h"
#include "SunLocator.h"
#include "TextureTile.h"
#include "global.h"

#include <QtGui/QImage>

#include <cmath>

namespace Marble
{

SunLightBlending::SunLightBlending()
    : Blending(),
      m_sunLocator( new SunLocator( new MarbleClock, new Planet( "earth" )))
{
}

SunLightBlending::~SunLightBlending()
{
    delete m_sunLocator;
}

void SunLightBlending::blend( QImage * const bottom, QSharedPointer<TextureTile> const & top ) const
{
    QImage const * const topImage = top->image();
    Q_ASSERT( topImage );
    Q_ASSERT( bottom->size() == topImage->size() );
    int const tileWidth = bottom->width();
    int const tileHeight = bottom->height();
    mDebug() << "SunLightBlending::blend, tile width/height:" << tileWidth << tileHeight;

    // number of pixels in current zoom level
    // TODO: fix calculation, take levelZero(Columns|Rows) into account
    int const globalWidth = tileWidth << top->id().zoomLevel();
    int const globalHeight = tileHeight << top->id().zoomLevel();
    mDebug() << "SunLightBlending::blend, global width/height:" << globalWidth << globalHeight;

    qreal const lonScale = 2.0 * M_PI / globalWidth;
    qreal const latScale = -M_PI / globalHeight;

    m_sunLocator->update();
    qreal const sunZenithLon = m_sunLocator->getLon() * DEG2RAD;
    qreal const sunZenithLat = m_sunLocator->getLat() * DEG2RAD;
    mDebug() << "SunLightBlending::blend, sun zenith lon/lat:" << sunZenithLon << sunZenithLat;

    // First we determine the supporting point interval for the interpolation.
    int const n = maxDivisor( 30, tileWidth );
    int const ipRight = n * (int)( tileWidth / n );

    for ( int y = 0; y < tileHeight; ++y ) {

        qreal const lat = latScale * ( top->id().y() * tileHeight + y ) - 0.5 * M_PI;
        qreal const a = sin(( lat + sunZenithLat ) / 2.0 );
        qreal const c = cos( lat ) * cos( -sunZenithLat );

        QRgb * bottomScanline = (QRgb*) bottom->scanLine( y );
        QRgb * topScanline = (QRgb*) topImage->scanLine( y );

        qreal shade = 0.0;
        qreal lastShade = -10.0;

        int x = 0;
        while ( x < tileWidth ) {
            bool const interpolate = ( x != 0 && x < ipRight && x + n < tileWidth );
            if ( interpolate ) {
                int const check = x + n;
                qreal const checkLon = lonScale * ( top->id().x() * tileWidth + check );
                shade = shading( checkLon - sunZenithLon, a, c );

                // if the shading didn't change across the interpolation
                // interval move on and don't change anything.
                if ( shade == lastShade && shade == 1.0 ) {
                    bottomScanline += n;
                    topScanline += n;
                    x += n;
                    continue;
                }
                if ( shade == lastShade && shade == 0.0 ) {
                    for ( int t = 0; t < n; ++t ) {
                        shadePixelComposite( *bottomScanline, *topScanline, shade );
                        ++bottomScanline;
                        ++topScanline;
                    }
                    x += n;
                    continue;
                }
                for ( int t = 0; t < n ; ++t ) {
                    qreal const lon = lonScale * ( top->id().x() * tileWidth + x );
                    shade = shading( lon - sunZenithLon, a, c );
                    shadePixelComposite( *bottomScanline, *topScanline, shade );
                    ++bottomScanline;
                    ++topScanline;
                    ++x;
                }
            }
            else {
                // Make sure we don't exceed the image memory
                if ( x < tileWidth ) {
                    qreal const lon = lonScale * ( top->id().x() * tileWidth + x );
                    shade = shading( lon - sunZenithLon, a, c );
                    shadePixelComposite( *bottomScanline, *topScanline, shade );
                    ++bottomScanline;
                    ++topScanline;
                    ++x;
                }
            }
            lastShade = shade;
        }
    }
}

void SunLightBlending::shadePixelComposite( QRgb & bottom, QRgb const top, qreal const brightness ) const
{
    if ( brightness > 0.99999 )
        // daylight - no change
        return;

    if ( brightness < 0.00001 ) {
        // night
        bottom = top;
    }
    else {
        // gradual shadowing
        int const bottomRed = qRed( bottom );
        int const bootomGreen = qGreen( bottom );
        int const bottomBlue = qBlue( bottom );

        int const topRed = qRed( top );
        int const topGreen = qGreen( top );
        int const topBlue = qBlue( top );

        bottom = qRgb( (int)( brightness * bottomRed + ( 1.0 - brightness ) * topRed ),
                       (int)( brightness * bootomGreen + ( 1.0 - brightness ) * topGreen ),
                       (int)( brightness * bottomBlue + ( 1.0 - brightness ) * topBlue ));
    }
}

// deltaLon = lon - sunLon
qreal SunLightBlending::shading( qreal const deltaLon, qreal const a, qreal const c ) const
{
    // haversine formula
    qreal const b = sin( deltaLon / 2.0 );
    qreal const h = ( a * a ) + c * ( b * b );

    /*
      h = 0.0 // directly beneath sun
      h = 0.5 // sunrise/sunset line
      h = 1.0 // opposite side of earth to the sun
      theta = 2*asin(sqrt(h))
    */

    // this equals 18 deg astronomical twilight and is correct for earth or venus
    qreal const twilightZone = 0.1;

    qreal brightness;
    if ( h <= 0.5 - twilightZone / 2.0 )
        brightness = 1.0;
    else if ( h >= 0.5 + twilightZone / 2.0 )
        brightness = 0.0;
    else
        brightness = ( 0.5 + twilightZone / 2.0 - h ) / twilightZone;

    return brightness;
}

// TODO: This should likely go into a math class in the future ...
int SunLightBlending::maxDivisor( int const maximum, int const fullLength ) const
{
    // Find the optimal interpolation interval n for the
    // current image canvas width
    int best = 2;

    int nEvalMin = fullLength;
    for ( int it = 1; it <= maximum; ++it ) {
        // The optimum is the interval which results in the least amount
        // supporting points taking into account the rest which can't
        // get used for interpolation.
        int nEval = fullLength / it + fullLength % it;
        if ( nEval < nEvalMin ) {
            nEvalMin = nEval;
            best = it;
        }
    }
    return best;
}

}
