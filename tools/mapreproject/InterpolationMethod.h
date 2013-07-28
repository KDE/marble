#ifndef INTERPOLATIONMETHOD_H
#define INTERPOLATIONMETHOD_H

#include <QColor>

class ReadOnlyMapImage;

class InterpolationMethod
{
public:
    explicit InterpolationMethod( ReadOnlyMapImage * const mapImage );
    virtual ~InterpolationMethod();

    virtual QRgb interpolate( double const x, double const y ) = 0;
    void setMapImage( ReadOnlyMapImage * const mapImage );

protected:
    ReadOnlyMapImage * m_mapImage;
};


inline void InterpolationMethod::setMapImage( ReadOnlyMapImage * const mapImage )
{
    m_mapImage = mapImage;
}

#endif
