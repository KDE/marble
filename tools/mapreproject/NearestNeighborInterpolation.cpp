#include "NearestNeighborInterpolation.h"

#include "ReadOnlyMapImage.h"

#include <cmath>

NearestNeighborInterpolation::NearestNeighborInterpolation( ReadOnlyMapImage * const mapImage )
    : InterpolationMethod( mapImage )
{
}

QRgb NearestNeighborInterpolation::interpolate( double const x, double const y )
{
    int const xr = round( x );
    int const yr = round( y );
    return m_mapImage->pixel( xr, yr );
}
