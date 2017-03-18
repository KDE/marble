#ifndef INTEGERINTERPOLATION_H
#define INTEGERINTERPOLATION_H

#include "InterpolationMethod.h"

class ReadOnlyMapImage;

class IntegerInterpolation: public InterpolationMethod
{
public:
    explicit IntegerInterpolation( ReadOnlyMapImage * const mapImage = NULL );

    QRgb interpolate( double const x, double const y ) override;
};

#endif
