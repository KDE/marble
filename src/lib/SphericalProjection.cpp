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
#include "SphericalProjection.h"

// Marble
#include "ViewportParams.h"


SphericalProjection::SphericalProjection()
    : AbstractProjection()
{
}

SphericalProjection::~SphericalProjection()
{
}


bool SphericalProjection::screenCoordinates( const double lon, const double lat,
                                             const ViewportParams *params,
                                             int& x, int& y,
					     CoordinateType coordType )
{
    Quaternion  p( lon, lat );
    if ( coordType == originalCoordinates )
	p.rotateAroundAxis( params->planetAxis().inverse() );
 
    x = (int)( params->width() / 2   + (double)( params->radius() ) * p.v[Q_X] );
    y = (int)( params->height() / 2  - (double)( params->radius() ) * p.v[Q_Y] );
 
    return p.v[Q_Z] > 0;
}

bool SphericalProjection::screenCoordinates( const GeoDataPoint &geopoint, 
                                             const ViewportParams *params,
                                             const matrix &planetAxisMatrix,
                                             int &x, int &y )
{
    double      absoluteAltitude = geopoint.altitude() + EARTH_RADIUS;
    Quaternion  qpos             = geopoint.quaternion();

    qpos.rotateAroundAxis( planetAxisMatrix );

    double      pixelAltitude = ( ( params->radius() ) 
                                  / EARTH_RADIUS * absoluteAltitude );
    if ( geopoint.altitude() < 10000 ) {
        // Skip placemarks at the other side of the earth.
        if ( qpos.v[Q_Z] < 0 )
            return false;
    }
    else {
        double  earthCenteredX = pixelAltitude * qpos.v[Q_X];
        double  earthCenteredY = pixelAltitude * qpos.v[Q_Y];
        double  radius         = params->radius();

        // Don't draw high placemarks (e.g. satellites) that aren't visible.
        if ( qpos.v[Q_Z] < 0
             && ( ( earthCenteredX * earthCenteredX
                    + earthCenteredY * earthCenteredY )
                  < radius * radius ) )
            return false;
    }

    // Let (x, y) be the position on the screen of the placemark..
    x = (int)(params->width()  / 2 + pixelAltitude * qpos.v[Q_X]);
    y = (int)(params->height() / 2 - pixelAltitude * qpos.v[Q_Y]);

    // Skip placemarks that are outside the screen area
    if ( x < 0 || x >= params->width() || y < 0 || y >= params->height() ) {
        return false;
    }

    return true;
}


bool SphericalProjection::geoCoordinates( const int x, const int y,
                                          const ViewportParams *params,
                                          double& lon, double& lat,
                                          GeoDataPoint::Unit unit )
{
    int           imgWidth2     = params->width() / 2;
    int           imgHeight2    = params->height() / 2;
    const double  inverseRadius = 1.0 / (double)(params->radius());
    bool          noerr         = false;

    if ( params->radius() > sqrt( ( x - imgWidth2 ) * ( x - imgWidth2 )
                                  + ( y - imgHeight2 ) * ( y - imgHeight2 ) ) )
    {
        double qx = inverseRadius * (double)( x - imgWidth2 );
        double qy = inverseRadius * (double)( imgHeight2 - y );
        double qr = 1.0 - qy * qy;

        double qr2z = qr - qx * qx;
        double qz   = ( qr2z > 0.0 ) ? sqrt( qr2z ) : 0.0;

        Quaternion  qpos( 0.0, qx, qy, qz );
        qpos.rotateAroundAxis( params->planetAxis() );
        qpos.getSpherical( lon, lat );

        noerr = true;
    }

    if ( unit == GeoDataPoint::Degree ) {
        lon *= RAD2DEG;
        lat *= RAD2DEG;
    }

    return noerr;
}

bool SphericalProjection::geoCoordinates( int x, int y, 
                                          const ViewportParams *params,
                                          Quaternion &q )
{
    // NYI
    return false;
}
