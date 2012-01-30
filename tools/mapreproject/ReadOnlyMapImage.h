#ifndef READONLYMAPIMAGE_H
#define READONLYMAPIMAGE_H

#include <QtGui/QColor>

class InterpolationMethod;

class ReadOnlyMapImage
{
public:
    virtual ~ReadOnlyMapImage();

    virtual QRgb pixel( double const lonRad, double const latRad ) = 0;
    virtual QRgb pixel( int const x, int const y ) = 0;
    virtual void setInterpolationMethod( InterpolationMethod * const interpolationMethod ) = 0;
};

#endif
