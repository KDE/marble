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
#include "MercatorProjection.h"

// Marble
#include "ViewportParams.h"

#ifdef Q_CC_MSVC
static double msvc_atanh(double x)
{
  return ( 0.5 * log( ( 1.0 + x ) / ( 1.0 - x ) ) );
}
#define atanh msvc_atanh
#endif

MercatorProjection::MercatorProjection()
    : AbstractProjection()
{
    // This is the max value where atanh( sin( lat ) ) is defined.
    m_maxLat  = 85.05113 * DEG2RAD;

    m_repeatX = true;
}

MercatorProjection::~MercatorProjection()
{
}


bool MercatorProjection::screenCoordinates( const double lon, const double lat,
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
 
    x = (int)( viewport->width()  / 2 + rad2Pixel * ( lon - centerLon ) );
    y = (int)( viewport->height() / 2 - rad2Pixel * ( atanh( sin( lat ) ) - centerLat ) );

    return true;
}

bool MercatorProjection::screenCoordinates( const GeoDataPoint &geopoint, 
                                            const ViewportParams *viewport,
                                            int &x, int &y, bool &globeHidesPoint )
{
    globeHidesPoint = false;
    double  lon;
    double  lat;
    double  rad2Pixel = 2 * viewport->radius() / M_PI;

    double  centerLon;
    double  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    geopoint.geoCoordinates( lon, lat );
    // FIXME: What is this magic number??
    //        Hmm, it must be the cutoff latitude, outside of which is not
    //        shown.  Create a const double of #define for this!
    if ( fabs( lat ) >=  85.05113 * DEG2RAD )
        return false;

    // Let (x, y) be the position on the screen of the placemark..
    x = (int)( viewport->width()  / 2 + rad2Pixel * ( lon - centerLon ) );
    y = (int)( viewport->height() / 2 - rad2Pixel * ( atanh( sin( lat ) ) - centerLat ) );

    // Skip placemarks that are outside the screen area.
    if ( ( y >= 0 && y < viewport->height() )
         && ( ( x >= 0 && x < viewport->width() ) 
              || (x - 4 * viewport->radius() >= 0
                  && x - 4 * viewport->radius() < viewport->width() )
              || (x + 4 * viewport->radius() >= 0
                  && x + 4 * viewport->radius() < viewport->width() ) ) )
    {
        return true;
    }

    return false;
}

bool MercatorProjection::screenCoordinates( const GeoDataPoint &geopoint, const ViewportParams * viewport, int *x, int &y, int &pointRepeatNum, bool &globeHidesPoint )
{
    // on flat projections the observer's view onto the point won't be 
    // obscured by the target planet itself
    globeHidesPoint = false;

    double  lon;
    double  lat;
    double  rad2Pixel = 2.0 * viewport->radius() / M_PI;

    double  centerLon;
    double  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    geopoint.geoCoordinates( lon, lat );

    if ( fabs( lat ) >=  maxLat() )
        return false;

    // Let (itX, y) be the first guess for one possible position on screen..
    int itX = (int)( viewport->width()  / 2.0 + rad2Pixel * ( lon - centerLon ) );
    y = (int)( viewport->height() / 2 - rad2Pixel * ( atanh( sin( lat ) ) - centerLat ) );

    // Make sure that the requested point is within the visible y range:
    if ( y >= 0 && y < viewport->height() ) {
        // First we deal with the case where the repetition doesn't happen
        if ( m_repeatX == false ) {
            *x = itX;
            if ( itX > 0 && itX < viewport->width() ) {
                return true;
            }
            else {
                // the requested point is out of the visible x range:
                return false;
            }
        }
        // For the repetition case the same geopoint gets displayed on 
        // the map many times.across the longitude.

        int xRepeatDistance = 4 * viewport->radius();

        // Finding the leftmost positive x value
        if ( itX > xRepeatDistance ) {
            itX %= xRepeatDistance;
        }
        if ( itX < 0 ) {
            itX += xRepeatDistance;
        }
        // the requested point is out of the visible x range:
        if ( itX > viewport->width() ) {
            return false;
        }

        // Now iterate through all visible x screen coordinates for the point 
        // from left to right.
        int itNum = 0;

        while ( itX < viewport->width() ) {
            *x = itX;
            ++x;
            ++itNum;
            itX += xRepeatDistance;
        }

        pointRepeatNum = itNum;

        return true;
    }

    // the requested point is out of the visible y range:
    return false;
}

bool MercatorProjection::geoCoordinates( const int x, const int y,
                                         const ViewportParams *viewport,
                                         double& lon, double& lat,
                                         GeoDataPoint::Unit unit )
{
    int           radius        = viewport->radius();
    int           imgWidth2     = viewport->width() / 2;
    int           imgHeight2    = viewport->height() / 2;
    const double  inverseRadius = 1.0 / (double)(radius);
    bool          noerr         = false;

    // Calculate translation of center point
    double  centerLon;
    double  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    int yCenterOffset = (int)( centerLat * (double)(2 * radius) / M_PI);
    int yTop          = imgHeight2 - radius + yCenterOffset;
    int yBottom       = yTop + 2 * radius;
    if ( y >= yTop && y < yBottom ) {
        int const  xPixels = x - imgWidth2;
        int const  yPixels = y - imgHeight2;

        double const pixel2rad = M_PI / (2 * radius);

        lat = atan( sinh( ((imgHeight2 + yCenterOffset) - y)
                          / (double)(2 * radius) * M_PI ) );
        lon = xPixels * pixel2rad + centerLon;

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

bool MercatorProjection::geoCoordinates( int x, int y, 
                                         const ViewportParams *viewport,
                                         Quaternion &q)
{
    // NYI
    return false;
}
