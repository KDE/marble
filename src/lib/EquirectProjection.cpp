//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>"
//


// Local
#include "EquirectProjection.h"

// Marble
#include "ViewportParams.h"


EquirectProjection::EquirectProjection()
    : AbstractProjection()
{
    m_maxLat  = 90.0 * DEG2RAD;

    m_repeatX = true;
}

EquirectProjection::~EquirectProjection()
{
}


bool EquirectProjection::screenCoordinates( const double lon, const double lat,
                                            const ViewportParams *viewport,
                                            int& x, int& y,
					    CoordinateType coordType )
{
    Q_UNUSED( coordType );

    // Calculate translation of center point
    double  centerLon;
    double  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );
    double  rad2Pixel = 2 * viewport->radius() / M_PI;
 
    x = (int)( viewport->width()  / 2 + ( lon - centerLon ) * rad2Pixel );
    y = (int)( viewport->height() / 2 - ( lat - centerLat ) * rad2Pixel );

    return true;
}

bool EquirectProjection::screenCoordinates( const GeoDataPoint &geopoint, 
                                            const ViewportParams *viewport,
                                            const matrix &planetAxisMatrix,
                                            int &x, int &y )
{
    double  lon;
    double  lat;
    double  rad2Pixel = 2 * viewport->radius() / M_PI;

    double  centerLon;
    double  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    geopoint.geoCoordinates( lon, lat );

    // Let (x, y) be the position on the screen of the placemark..
    x = (int)(viewport->width()  / 2 + rad2Pixel * (lon - centerLon));
    y = (int)(viewport->height() / 2 - rad2Pixel * (lat - centerLat));

    // Skip placemarks that are outside the screen area
    //
    if ( ( y >= 0 && y < viewport->height() )
         && ( ( x >= 0 && x < viewport->width() )
              || ( x - 4 * viewport->radius() >= 0
                   && x - 4 * viewport->radius() < viewport->width() )
              || ( x + 4 * viewport->radius() >= 0
                   && x + 4 * viewport->radius() < viewport->width() ) ) )
    {
        return true;
    }

    return false;
}


bool EquirectProjection::geoCoordinates( const int x, const int y,
                                         const ViewportParams *viewport,
                                         double& lon, double& lat,
                                         GeoDataPoint::Unit unit )
{
    int   radius     = viewport->radius();
    int   imgWidth2  = viewport->width() / 2;
    int   imgHeight2 = viewport->height() / 2;
    bool  noerr      = false;

    // Calculate translation of center point
    double  centerLon;
    double  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    int yCenterOffset =  (int)( centerLat * (double)(2 * radius) / M_PI);
    int yTop          = imgHeight2 - radius + yCenterOffset;
    int yBottom       = yTop + 2 * radius;
    if ( y >= yTop && y < yBottom ) {
        int const xPixels = x - imgWidth2;
        int const yPixels = y - imgHeight2;

        double const pixel2rad = M_PI / (2 * radius);
        lat = - yPixels * pixel2rad + centerLat;
        lon = + xPixels * pixel2rad + centerLon;

        while ( lon > M_PI )  lon -= 2*M_PI;
        while ( lon < -M_PI ) lon += 2*M_PI;

        noerr = true;
    }

    if ( unit == GeoDataPoint::Degree ) {
        lon *= RAD2DEG;
        lat *= RAD2DEG;
    }

    return noerr;
}

bool EquirectProjection::geoCoordinates( int x, int y, 
                                         const ViewportParams *viewport,
                                         Quaternion &q )
{
    // NYI
    return false;
}
