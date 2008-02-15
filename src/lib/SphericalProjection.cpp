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
                                             ViewportParams *params,
                                             int& x, int& y )
{
    Quaternion p(lon * DEG2RAD, lat * DEG2RAD);
    p.rotateAroundAxis(params->planetAxis().inverse());
 
    x = (int)( params->width() / 2   + (double)( params->radius() ) * p.v[Q_X] );
    y = (int)( params->height() / 2  + (double)( params->radius() ) * p.v[Q_Y] );
 
    return p.v[Q_Z] > 0;
}

bool SphericalProjection::geoCoordinates( const int x, const int y,
                                          ViewportParams *params,
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
                                          ViewportParams *params,
                                          Quaternion &q)
{
    // NYI
    return false;
}
