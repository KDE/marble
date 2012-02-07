#ifndef BILINEARINTERPOLATION_H
#define BILINEARINTERPOLATION_H

#include "InterpolationMethod.h"

class ReadOnlyMapImage;

class BilinearInterpolation: public InterpolationMethod
{
public:
    explicit BilinearInterpolation( ReadOnlyMapImage * const mapImage = NULL );

    virtual QRgb interpolate( double const x, double const y );
};

#endif
