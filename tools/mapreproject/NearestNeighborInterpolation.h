#ifndef NEARESTNEIGHBORINTERPOLATION_H
#define NEARESTNEIGHBORINTERPOLATION_H

#include "InterpolationMethod.h"

class ReadOnlyMapImage;

class NearestNeighborInterpolation: public InterpolationMethod
{
public:
    explicit NearestNeighborInterpolation( ReadOnlyMapImage * const mapImage = NULL );

    virtual QRgb interpolate( double const x, double const y );
};

#endif
