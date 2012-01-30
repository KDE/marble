#ifndef INTERPOLATIONMETHOD_H
#define INTERPOLATIONMETHOD_H

#include <QtGui/QColor>

class ReadOnlyMapImage;

class InterpolationMethod
{
public:
    virtual ~InterpolationMethod();

    virtual QRgb interpolate( double const x, double const y ) = 0;
    virtual void setMapImage( ReadOnlyMapImage * mapImage ) = 0;
};

#endif
