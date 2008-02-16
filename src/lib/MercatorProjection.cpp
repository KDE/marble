//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


// Local
#include "MercatorProjection.h"

// Marble
#include "ViewportParams.h"


MercatorProjection::MercatorProjection()
    : AbstractProjection()
{
}

MercatorProjection::~MercatorProjection()
{
}


bool MercatorProjection::screenCoordinates( const double lon, const double lat,
                                            const ViewportParams *params,
                                            int& x, int& y )
{
    // Calculate translation of center point
    double  centerLon;
    double  centerLat;
    params->centerCoordinates( centerLon, centerLat );
    double  rad2Pixel = 2 * params->radius() / M_PI;
 
    x = (int)( params->width()  / 2 + ( lon * DEG2RAD + centerLon ) * rad2Pixel );
    y = (int)( params->height() / 2 + rad2Pixel * (centerLat - atanh( sin( lat ) ) ) );

    return true;
}

bool MercatorProjection::geoCoordinates( const int x, const int y,
                                         const ViewportParams *params,
                                         double& lon, double& lat,
                                         GeoDataPoint::Unit unit )
{
    int           imgWidth2     = params->width() / 2;
    int           imgHeight2    = params->height() / 2;
    const double  inverseRadius = 1.0 / (double)(params->radius());
    bool          noerr         = false;

    // Calculate translation of center point
    double  centerLon;
    double  centerLat;
    params->centerCoordinates( centerLon, centerLat );

    int yCenterOffset =  (int)((double)(2 * params->radius())
                               / M_PI * centerLat);
    int yTop          = imgHeight2 - params->radius() + yCenterOffset;
    int yBottom       = yTop + 2 * params->radius();
    if ( y >= yTop && y < yBottom ) {
        int const  xPixels = x - imgWidth2;
        int const  yPixels = y - imgHeight2;

        double const pixel2rad = M_PI / (2 * params->radius());

        lat = atan( sinh( ((imgHeight2 + yCenterOffset) - y)
                          / (double)(2 * params->radius()) * M_PI ) );
        // FIXME: This looks strange with the lonesome +
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

bool MercatorProjection::geoCoordinates( int x, int y, 
                                         const ViewportParams *params,
                                         Quaternion &q)
{
    // NYI
    return false;
}
