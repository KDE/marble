#ifndef SIMPLEMAPIMAGE_H
#define SIMPLEMAPIMAGE_H

#include "ReadOnlyMapImage.h"

#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtGui/QImage>

class InterpolationMethod;

class SimpleMapImage: public ReadOnlyMapImage
{
public:
    explicit SimpleMapImage( QString const & fileName );

    virtual QRgb pixel( double const lonRad, double const latRad );
    virtual QRgb pixel( int const x, int const y );
    virtual void setInterpolationMethod( InterpolationMethod * const interpolationMethod );

private:
    double lonRadToPixelX( double const lonRad ) const;
    double latRadToPixelY( double const latRad ) const;

    QImage m_image;
    int m_mapWidthPixel;
    int m_mapHeightPixel;
    InterpolationMethod * m_interpolationMethod;
};

#endif
