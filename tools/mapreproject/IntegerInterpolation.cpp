#include "IntegerInterpolation.h"

#include "ReadOnlyMapImage.h"

IntegerInterpolation::IntegerInterpolation( ReadOnlyMapImage * const mapImage )
    : InterpolationMethod( mapImage )
{
}

QRgb IntegerInterpolation::interpolate( double const x, double const y )
{
    return m_mapImage->pixel( static_cast<int>( x ), static_cast<int>( y ));
}
