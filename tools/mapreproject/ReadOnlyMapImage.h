#ifndef READONLYMAPIMAGE_H
#define READONLYMAPIMAGE_H

#include <QtGui/QColor>

class ReadOnlyMapImage
{
public:
    virtual ~ReadOnlyMapImage();

    virtual QRgb pixel( double const lonRad, double const latRad ) = 0;
};

#endif
