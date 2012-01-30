#include "SimpleMapImage.h"

#include "InterpolationMethod.h"

#include <cmath>

SimpleMapImage::SimpleMapImage( QString const & fileName )
    : m_image( fileName ),
      m_mapWidthPixel( m_image.width() ),
      m_mapHeightPixel( m_image.height() )
{
    if ( m_image.isNull() )
        qFatal( "Invalid image '%s'", fileName.toStdString().c_str() );
}

QRgb SimpleMapImage::pixel( double const lonRad,  double const latRad )
{
    double const x = lonRadToPixelX( lonRad );
    double const y = latRadToPixelY( latRad );
    return m_interpolationMethod->interpolate( x, y );
}

QRgb SimpleMapImage::pixel( int const x, int const y )
{
    return m_image.pixel( x, m_mapHeightPixel - y );
}

void SimpleMapImage::setInterpolationMethod( InterpolationMethod * const interpolationMethod )
{
    m_interpolationMethod = interpolationMethod;
    m_interpolationMethod->setMapImage( this );
}

inline double SimpleMapImage::lonRadToPixelX( double const lonRad ) const
{
    return static_cast<double>( m_mapWidthPixel ) / ( 2.0 * M_PI ) * lonRad
            + 0.5 * static_cast<double>( m_mapWidthPixel );
}

inline double SimpleMapImage::latRadToPixelY( double const latRad ) const
{
    return static_cast<double>( m_mapHeightPixel ) / M_PI * latRad
            + 0.5 * static_cast<double>( m_mapHeightPixel );
}
