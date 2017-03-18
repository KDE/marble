#ifndef SIMPLEMAPIMAGE_H
#define SIMPLEMAPIMAGE_H

#include "ReadOnlyMapImage.h"

#include <QString>
#include <QColor>
#include <QImage>

class InterpolationMethod;

class SimpleMapImage: public ReadOnlyMapImage
{
public:
    explicit SimpleMapImage( QString const & fileName );

    QRgb pixel( double const lonRad, double const latRad ) override;
    QRgb pixel( int const x, int const y ) override;
    void setInterpolationMethod( InterpolationMethod * const interpolationMethod ) override;

private:
    double lonRadToPixelX( double const lonRad ) const;
    double latRadToPixelY( double const latRad ) const;

    QImage m_image;
    int m_mapWidthPixel;
    int m_mapHeightPixel;
    InterpolationMethod * m_interpolationMethod;
};

#endif
