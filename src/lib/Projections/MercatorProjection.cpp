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
#include "MercatorProjectionHelper.h"
#include "ViewportParams.h"
#include "MathHelper.h"
#include "GeoDataPoint.h"

using namespace Marble;

static MercatorProjectionHelper  theHelper;


MercatorProjection::MercatorProjection()
    : AbstractProjection()
{
    // This is the max value where atanh( sin( lat ) ) is defined.
    m_maxLat  = 85.05113 * DEG2RAD;
    m_minLat  = -85.05113 * DEG2RAD;
    m_traversableMaxLat = false;
    m_repeatX = true;
}

MercatorProjection::~MercatorProjection()
{
}


AbstractProjectionHelper *MercatorProjection::helper()
{
    return &theHelper;
}


bool MercatorProjection::screenCoordinates( const qreal lon, const qreal lat,
                                            const ViewportParams *viewport,
                                            int& x, int& y )
{
    if ( fabs( lat ) > m_maxLat )
        return false;

    // Convenience variables
    int  radius = viewport->radius();
    int  width  = viewport->width();
    int  height = viewport->height();

    qreal  rad2Pixel = 2 * radius / M_PI;

    // Calculate translation of center point
    qreal  centerLon;
    qreal  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    // Let (x, y) be the position on the screen of the placemark..
    x = (int)( width  / 2 + rad2Pixel * ( lon - centerLon ) );
    y = (int)( height / 2 - rad2Pixel * ( atanh( sin( lat ) ) - atanh( sin( centerLat ) ) ) );

    return true;
}

bool MercatorProjection::screenCoordinates( const GeoDataCoordinates &geopoint, 
                                            const ViewportParams *viewport,
                                            int &x, int &y, bool &globeHidesPoint )
{
    globeHidesPoint = false;
    qreal  lon;
    qreal  lat;

    geopoint.geoCoordinates( lon, lat );

    if ( fabs( lat ) >  m_maxLat )
        return false;

    // Convenience variables
    int  radius = viewport->radius();
    int  width  = viewport->width();
    int  height = viewport->height();

    qreal  rad2Pixel = 2 * radius / M_PI;

    qreal  centerLon;
    qreal  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );
    
    // Let (x, y) be the position on the screen of the placemark..
    x = (int)( width  / 2 + rad2Pixel * ( lon - centerLon ) );
    y = (int)( height / 2 - rad2Pixel * ( atanh( sin( lat ) ) - atanh( sin( centerLat ) ) ) );

    // Return true if the calculated point is inside the screen area,
    // otherwise return false.
    return ( ( 0 <= y && y < height )
             && ( ( 0 <= x && x < width )
                  || ( 0 <= x - 4 * radius && x - 4 * radius < width )
                  || ( 0 <= x + 4 * radius && x + 4 * radius < width ) ) );
}

bool MercatorProjection::screenCoordinates( const GeoDataCoordinates &coordinates,
                                            const ViewportParams *viewport,
                                            int *x, int &y, int &pointRepeatNum,
                                            bool &globeHidesPoint )
{
    // On flat projections the observer's view onto the point won't be 
    // obscured by the target planet itself.
    globeHidesPoint = false;

    qreal  lon;
    qreal  lat;

    coordinates.geoCoordinates( lon, lat );

    if ( fabs( lat ) > maxLat() )
        return false;

    // Convenience variables
    int  radius = viewport->radius();
    int  width  = viewport->width();
    int  height = viewport->height();

    qreal  rad2Pixel = 2.0 * radius / M_PI;

    qreal  centerLon;
    qreal  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    // Let (itX, y) be the first guess for one possible position on screen..
    int itX = (int)( width  / 2.0 + rad2Pixel * ( lon - centerLon ) );
    y = (int)( height / 2 - rad2Pixel * ( atanh( sin( lat ) ) - atanh( sin( centerLat ) ) ) );

    // Make sure that the requested point is within the visible y range:
    if ( 0 <= y && y < height ) {
        // First we deal with the case where the repetition doesn't happen
        if ( m_repeatX == false ) {
            *x = itX;
            if ( 0 < itX && itX < width ) {
                return true;
            }
            else {
                // the requested point is out of the visible x range:
                return false;
            }
        }
        // For the repetition case the same geopoint gets displayed on 
        // the map many times.across the longitude.

        int xRepeatDistance = 4 * radius;

        // Finding the leftmost positive x value
        if ( itX > xRepeatDistance ) {
            itX %= xRepeatDistance;
        }
        if ( itX < 0 ) {
            itX += xRepeatDistance;
        }
        // the requested point is out of the visible x range:
        if ( itX > width ) {
            return false;
        }

        // Now iterate through all visible x screen coordinates for the point 
        // from left to right.
        int itNum = 0;

        while ( itX < width ) {
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

bool MercatorProjection::screenCoordinates( const GeoDataLineString &lineString, 
                                    const ViewportParams *viewport,
                                    QVector<QPolygonF *> &polygons )
{
    return false;
}

bool MercatorProjection::screenCoordinates( const GeoDataLinearRing &linearRing, 
                                    const ViewportParams *viewport,
                                    QVector<QPolygonF *> &polygons )
{
    return false;
}

bool MercatorProjection::geoCoordinates( int x, int y,
                                         const ViewportParams *viewport,
                                         qreal& lon, qreal& lat,
                                         GeoDataCoordinates::Unit unit )
{
    int           radius             = viewport->radius();
    int           halfImageWidth     = viewport->width() / 2;
    int           halfImageHeight    = viewport->height() / 2;
    bool          noerr              = false;

    // Calculate translation of center point
    qreal  centerLon;
    qreal  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    // Calculate how many pixel are being represented per radians.
    const float rad2Pixel = (qreal)( 2 * radius )/M_PI;

    int yCenterOffset = (int)( asinh( tan( centerLat ) ) * rad2Pixel  );
    int yTop          = halfImageHeight - 2 * radius + yCenterOffset;
    int yBottom       = yTop + 4 * radius;

    if ( y >= yTop && y < yBottom ) {
        int    const  xPixels   = x - halfImageWidth;
        qreal const  pixel2Rad = M_PI / (2 * radius);

        lat = atan( sinh( ( ( halfImageHeight + yCenterOffset ) - y)
                          * pixel2Rad ) );
        lon = xPixels * pixel2Rad + centerLon;

        while ( lon > M_PI )  lon -= 2*M_PI;
        while ( lon < -M_PI ) lon += 2*M_PI;

        noerr = true;
    }

    if ( unit == GeoDataCoordinates::Degree ) {
        lon *= RAD2DEG;
        lat *= RAD2DEG;
    }

    return noerr;
}


GeoDataLatLonAltBox MercatorProjection::latLonAltBox( const QRect& screenRect,
                                                      const ViewportParams *viewport )
{
    // For the case where the whole viewport gets covered there is a 
    // pretty dirty and generic detection algorithm:
    GeoDataLatLonAltBox latLonAltBox = AbstractProjection::latLonAltBox( screenRect, viewport );

    // The remaining algorithm should be pretty generic for all kinds of 
    // flat projections:

    // If the whole globe is visible we can easily calculate
    // analytically the lon-/lat- range.
    qreal pitch = GeoDataPoint::normalizeLat( viewport->planetAxis().pitch() );

    if ( m_repeatX ) {
        int xRepeatDistance = 4 * viewport->radius();
        if ( viewport->width() >= xRepeatDistance ) {
            latLonAltBox.setWest( -M_PI );
            latLonAltBox.setEast( +M_PI );
        }
    }
    else {
        // We need a point on the screen at maxLat that definetely
        // gets displayed:
        qreal averageLatitude = ( latLonAltBox.north() + latLonAltBox.south() ) / 2.0;
    
        GeoDataCoordinates maxLonPoint( +M_PI, averageLatitude, GeoDataCoordinates::Radian );
        GeoDataCoordinates minLonPoint( -M_PI, averageLatitude, GeoDataCoordinates::Radian );
    
        int dummyX, dummyY; // not needed
        bool dummyVal;
    
        if ( screenCoordinates( maxLonPoint, viewport, dummyX, dummyY, dummyVal ) ) {
            latLonAltBox.setEast( +M_PI );
        }
        if ( screenCoordinates( minLonPoint, viewport, dummyX, dummyY, dummyVal ) ) {
            latLonAltBox.setWest( -M_PI );
        }
    }

//    qDebug() << latLonAltBox.text( GeoDataCoordinates::Degree );

    return latLonAltBox;
}


bool MercatorProjection::mapCoversViewport( const ViewportParams *viewport ) const
{
    int           radius = viewport->radius();
    int           height = viewport->height();

    // Calculate translation of center point
    qreal  centerLon;
    qreal  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    // Calculate how many pixel are being represented per radians.
    const float rad2Pixel = (float)( 2 * radius )/M_PI;

    int yCenterOffset = (int)( asinh( tan( centerLat ) ) * rad2Pixel  );
    int yTop          = height / 2 - 2 * radius + yCenterOffset;
    int yBottom       = yTop + 4 * radius;

    if ( yTop >= 0 || yBottom < height )
        return false;

    return true;
}
