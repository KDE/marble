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
#include "SphericalProjection.h"

// Marble
#include "SphericalProjectionHelper.h"
#include "ViewportParams.h"


static SphericalProjectionHelper  theHelper;


SphericalProjection::SphericalProjection()
    : AbstractProjection()
{
    m_maxLat  = 90.0 * DEG2RAD;

    m_repeatX = false;
}

SphericalProjection::~SphericalProjection()
{
}


AbstractProjectionHelper *SphericalProjection::helper()
{
    return &theHelper;
}


bool SphericalProjection::screenCoordinates( const double lon, const double lat,
                                             const ViewportParams *viewport,
                                             int& x, int& y,
					     CoordinateType coordType )
{
    Quaternion  p( lon, lat );
    if ( coordType == originalCoordinates )
	p.rotateAroundAxis( viewport->planetAxis().inverse() );
 
    x = (int)( viewport->width()  / 2 + (double)( viewport->radius() ) * p.v[Q_X] );
    y = (int)( viewport->height() / 2 - (double)( viewport->radius() ) * p.v[Q_Y] );
 
    return p.v[Q_Z] > 0;
}

bool SphericalProjection::screenCoordinates( const GeoDataPoint &geopoint, 
                                             const ViewportParams *viewport,
                                             int &x, int &y, bool &globeHidesPoint )
{
    double      absoluteAltitude = geopoint.altitude() + EARTH_RADIUS;
    Quaternion  qpos             = geopoint.quaternion();

    qpos.rotateAroundAxis( *( viewport->planetAxisMatrix() ) );

    double      pixelAltitude = ( ( viewport->radius() ) 
                                  / EARTH_RADIUS * absoluteAltitude );
    if ( geopoint.altitude() < 10000 ) {
        // Skip placemarks at the other side of the earth.
        if ( qpos.v[Q_Z] < 0 ) {
            globeHidesPoint = true;
            return false;
        }
    }
    else {
        double  earthCenteredX = pixelAltitude * qpos.v[Q_X];
        double  earthCenteredY = pixelAltitude * qpos.v[Q_Y];
        double  radius         = viewport->radius();

        // Don't draw high placemarks (e.g. satellites) that aren't visible.
        if ( qpos.v[Q_Z] < 0
             && ( ( earthCenteredX * earthCenteredX
                    + earthCenteredY * earthCenteredY )
                  < radius * radius ) ) {
            globeHidesPoint = true;
            return false;
        }
    }

    // Let (x, y) be the position on the screen of the placemark..
    x = (int)(viewport->width()  / 2 + pixelAltitude * qpos.v[Q_X]);
    y = (int)(viewport->height() / 2 - pixelAltitude * qpos.v[Q_Y]);

    // Skip placemarks that are outside the screen area
    if ( x < 0 || x >= viewport->width() || y < 0 || y >= viewport->height() ) {
        globeHidesPoint = false;
        return false;
    }

    globeHidesPoint = false;
    return true;
}

bool SphericalProjection::screenCoordinates( const GeoDataPoint &geopoint, const ViewportParams *viewport, int *x, int &y, int& pointRepeatNum, bool &globeHidesPoint )
{
    double      absoluteAltitude = geopoint.altitude() + EARTH_RADIUS;
    Quaternion  qpos             = geopoint.quaternion();

    qpos.rotateAroundAxis( *( viewport->planetAxisMatrix() ) );

    double      pixelAltitude = ( ( viewport->radius() ) 
                                  / EARTH_RADIUS * absoluteAltitude );
    if ( geopoint.altitude() < 10000 ) {
        // Skip placemarks at the other side of the earth.
        if ( qpos.v[Q_Z] < 0 ) {
            globeHidesPoint = true;
            return false;
        }
    }
    else {
        double  earthCenteredX = pixelAltitude * qpos.v[Q_X];
        double  earthCenteredY = pixelAltitude * qpos.v[Q_Y];
        double  radius         = viewport->radius();

        // Don't draw high placemarks (e.g. satellites) that aren't visible.
        if ( qpos.v[Q_Z] < 0
             && ( ( earthCenteredX * earthCenteredX
                    + earthCenteredY * earthCenteredY )
                  < radius * radius ) ) {
            globeHidesPoint = true;
            return false;
        }
    }

    // Let (x, y) be the position on the screen of the placemark..
    *x = (int)(viewport->width()  / 2 + pixelAltitude * qpos.v[Q_X]);
    y = (int)(viewport->height() / 2 - pixelAltitude * qpos.v[Q_Y]);

    // Skip placemarks that are outside the screen area
    if ( *x < 0 || *x >= viewport->width() || y < 0 || y >= viewport->height() ) {
        globeHidesPoint = false;
        return false;
    }

    // This projection doesn't have any repetitions, 
    // so the number of screen points referring to the geopoint is one.
    pointRepeatNum = 1;
    globeHidesPoint = false;
    return true;
}

bool SphericalProjection::geoCoordinates( const int x, const int y,
                                          const ViewportParams *viewport,
                                          double& lon, double& lat,
                                          GeoDataPoint::Unit unit )
{
    int           imgWidth2     = viewport->width() / 2;
    int           imgHeight2    = viewport->height() / 2;
    const double  inverseRadius = 1.0 / (double)(viewport->radius());
    bool          noerr         = false;

    if ( viewport->radius() > sqrt( (double)(( x - imgWidth2 ) * ( x - imgWidth2 )
                                  + ( y - imgHeight2 ) * ( y - imgHeight2 )) ) )
    {
        double qx = inverseRadius * (double)( x - imgWidth2 );
        double qy = inverseRadius * (double)( imgHeight2 - y );
        double qr = 1.0 - qy * qy;

        double qr2z = qr - qx * qx;
        double qz   = ( qr2z > 0.0 ) ? sqrt( qr2z ) : 0.0;

        Quaternion  qpos( 0.0, qx, qy, qz );
        qpos.rotateAroundAxis( viewport->planetAxis() );
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
                                          const ViewportParams *viewport,
                                          Quaternion &q )
{
    // NYI
    return false;
}
