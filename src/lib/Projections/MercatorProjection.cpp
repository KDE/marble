//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
//


// Local
#include "MercatorProjection.h"

#include "MarbleDebug.h"

// Marble
#include "ViewportParams.h"

#include "MathHelper.h"
#include "GeoDataPoint.h"

using namespace Marble;

MercatorProjection::MercatorProjection()
    : AbstractProjection(),
      d( 0 )
{
    setRepeatX( repeatableX() );
    setMinLat( minValidLat() );
    setMaxLat( maxValidLat() );
}

MercatorProjection::~MercatorProjection()
{
}

bool MercatorProjection::repeatableX() const
{
    return true;
}

qreal MercatorProjection::maxValidLat() const
{
    // This is the max value where atanh( sin( lat ) ) is defined.
    return +85.05113 * DEG2RAD;
}

qreal MercatorProjection::minValidLat() const
{
    // This is the min value where atanh( sin( lat ) ) is defined.
    return -85.05113 * DEG2RAD;
}

bool MercatorProjection::screenCoordinates( const qreal lon, const qreal _lat,
                                            const ViewportParams *viewport,
                                            qreal& x, qreal& y ) const
{
    bool retval = true;
    qreal lat = _lat;
    
    if ( lat > maxLat() ) {
        lat = maxLat();
        retval = false;
    }
    if ( lat < minLat() ) {
        lat = minLat();
        retval = false;
    }

    // Convenience variables
    int  radius = viewport->radius();
    qreal  width  = (qreal)(viewport->width());
    qreal  height = (qreal)(viewport->height());

    qreal  rad2Pixel = 2 * radius / M_PI;

    // Calculate translation of center point
    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();

    // Let (x, y) be the position on the screen of the placemark..
    x = ( width  / 2 + rad2Pixel * ( lon - centerLon ) );
    y = ( height / 2 - rad2Pixel * ( atanh( sin( lat ) ) - atanh( sin( centerLat ) ) ) );

    return retval && ( ( 0 <= y && y < height )
                  && ( ( 0 <= x && x < width )
                  || ( 0 <= x - 4 * radius && x - 4 * radius < width )
                  || ( 0 <= x + 4 * radius && x + 4 * radius < width ) ) );
}

bool MercatorProjection::screenCoordinates( const GeoDataCoordinates &geopoint, 
                                            const ViewportParams *viewport,
                                            qreal &x, qreal &y, bool &globeHidesPoint ) const
{
    globeHidesPoint = false;
    qreal  lon;
    qreal  lat;

    geopoint.geoCoordinates( lon, lat );

    bool retval = true;

    if ( lat > maxLat() ) {
        GeoDataCoordinates approxCoords( geopoint );
        approxCoords.setLatitude( maxLat() );
        approxCoords.geoCoordinates( lon, lat );
        retval = false;
    }

    if ( lat < minLat() ) {
        GeoDataCoordinates approxCoords( geopoint );
        approxCoords.setLatitude( minLat() );
        approxCoords.geoCoordinates( lon, lat );
        retval = false;
    }

    // Convenience variables
    int  radius = viewport->radius();
    qreal  width  = (qreal)(viewport->width());
    qreal  height = (qreal)(viewport->height());

    qreal  rad2Pixel = 2 * radius / M_PI;

    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();

    // Let (x, y) be the position on the screen of the placemark..
    x = ( width  / 2 + rad2Pixel * ( lon - centerLon ) );
    y = ( height / 2 - rad2Pixel * ( atanh( sin( lat ) ) - atanh( sin( centerLat ) ) ) );

    // Return true if the calculated point is inside the screen area,
    // otherwise return false.
    return retval && ( ( 0 <= y && y < height )
                  && ( ( 0 <= x && x < width )
                  || ( 0 <= x - 4 * radius && x - 4 * radius < width )
                  || ( 0 <= x + 4 * radius && x + 4 * radius < width ) ) );
}

bool MercatorProjection::screenCoordinates( const GeoDataCoordinates &coordinates,
                                            const ViewportParams *viewport,
                                            qreal *x, qreal &y, int &pointRepeatNum,
                                            const QSizeF& size,
                                            bool &globeHidesPoint ) const
{
    // On flat projections the observer's view onto the point won't be 
    // obscured by the target planet itself.
    globeHidesPoint = false;

    qreal  lon;
    qreal  lat;

    coordinates.geoCoordinates( lon, lat );

    bool retval = true;

    if ( lat > maxLat() ) {
        GeoDataCoordinates approxCoords( coordinates );
        approxCoords.setLatitude( maxLat() );
        approxCoords.geoCoordinates( lon, lat );
        retval = false;
    }

    if ( lat < minLat() ) {
        GeoDataCoordinates approxCoords( coordinates );
        approxCoords.setLatitude( minLat() );
        approxCoords.geoCoordinates( lon, lat );
        retval = false;
    }

    // Convenience variables
    int  radius = viewport->radius();
    qreal  width  = (qreal)(viewport->width());
    qreal  height = (qreal)(viewport->height());

    qreal  rad2Pixel = 2.0 * radius / M_PI;

    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();

    // Let (itX, y) be the first guess for one possible position on screen..
    qreal itX = ( width  / 2.0 + rad2Pixel * ( lon - centerLon ) );
    y = ( height / 2.0 - rad2Pixel * ( atanh( sin( lat ) ) - atanh( sin( centerLat ) ) ) );

    // Make sure that the requested point is within the visible y range:
    if ( 0 <= y + size.height() / 2.0 && y < height + size.height() / 2.0 ) {
        // First we deal with the case where the repetition doesn't happen
        if ( !repeatX() ) {
            *x = itX;
            if ( 0 < itX + size.width() / 2.0  && itX < width + size.width() / 2.0 ) {
                return retval && true;
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
            int repeatNum = (int)( itX / xRepeatDistance );  
            itX = itX - repeatNum * xRepeatDistance;
        }
        if ( itX + size.width() / 2.0 < 0 ) {
            itX += xRepeatDistance;
        }
        // the requested point is out of the visible x range:
        if ( itX > width + size.width() / 2.0 ) {
            return false;
        }

        // Now iterate through all visible x screen coordinates for the point 
        // from left to right.
        int itNum = 0;
        while ( itX - size.width() / 2.0 < width ) {
            *x = itX;
            ++x;
            ++itNum;
            itX += xRepeatDistance;
        }

        pointRepeatNum = itNum;

        return retval && true;
    }

    // the requested point is out of the visible y range:
    return false;
}


bool MercatorProjection::geoCoordinates( const int x, const int y,
                                         const ViewportParams *viewport,
                                         qreal& lon, qreal& lat,
                                         GeoDataCoordinates::Unit unit ) const
{
    const int radius = viewport->radius();

    // Calculate translation of center point
    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();

    // Calculate how many pixel are being represented per radians.
    const float rad2Pixel = (qreal)( 2 * radius )/M_PI;
    const qreal pixel2Rad = M_PI / (2 * radius);

    {
        const int halfImageWidth = viewport->width() / 2;
        const int xPixels = x - halfImageWidth;
        lon = xPixels * pixel2Rad + centerLon;

        while ( lon > M_PI )  lon -= 2*M_PI;
        while ( lon < -M_PI ) lon += 2*M_PI;

        if ( unit == GeoDataCoordinates::Degree ) {
            lon *= RAD2DEG;
        }
    }

    {
        const int halfImageHeight    = viewport->height() / 2;
        const int yCenterOffset = (int)( asinh( tan( centerLat ) ) * rad2Pixel  );
        const int yTop          = halfImageHeight - 2 * radius + yCenterOffset;
        const int yBottom       = yTop + 4 * radius;
        if ( y >= yTop && y < yBottom ) {
            lat = atan( sinh( ( ( halfImageHeight + yCenterOffset ) - y)
                              * pixel2Rad ) );

            if ( unit == GeoDataCoordinates::Degree ) {
                lat *= RAD2DEG;
            }

            return true; // lat successfully calculated
        }
    }

    return false; // lat unchanged
}


GeoDataLatLonAltBox MercatorProjection::latLonAltBox( const QRect& screenRect,
                                                      const ViewportParams *viewport ) const
{
    qreal west;
    qreal north = 85*DEG2RAD;
    geoCoordinates( screenRect.left(), screenRect.top(), viewport, west, north, GeoDataCoordinates::Radian );

    qreal east;
    qreal south = -85*DEG2RAD;
    geoCoordinates( screenRect.right(), screenRect.bottom(), viewport, east, south, GeoDataCoordinates::Radian );

    // For the case where the whole viewport gets covered there is a
    // pretty dirty and generic detection algorithm:
    GeoDataLatLonAltBox latLonAltBox;
    latLonAltBox.setNorth( north, GeoDataCoordinates::Radian );
    latLonAltBox.setSouth( south, GeoDataCoordinates::Radian );
    latLonAltBox.setWest( west, GeoDataCoordinates::Radian );
    latLonAltBox.setEast( east, GeoDataCoordinates::Radian );
    latLonAltBox.setMinAltitude(      -100000000.0 );
    latLonAltBox.setMaxAltitude( 100000000000000.0 );

    // The remaining algorithm should be pretty generic for all kinds of 
    // flat projections:

    // If the whole globe is visible we can easily calculate
    // analytically the lon-/lat- range.
    // qreal pitch = GeoDataPoint::normalizeLat( viewport->planetAxis().pitch() );

    if ( repeatX() ) {
        int xRepeatDistance = 4 * viewport->radius();
        if ( viewport->width() >= xRepeatDistance ) {
            latLonAltBox.setWest( -M_PI );
            latLonAltBox.setEast( +M_PI );
        }
    }
    else {
        // We need a point on the screen at maxLat that definitely
        // gets displayed:
        qreal averageLatitude = ( latLonAltBox.north() + latLonAltBox.south() ) / 2.0;
    
        GeoDataCoordinates maxLonPoint( +M_PI, averageLatitude, GeoDataCoordinates::Radian );
        GeoDataCoordinates minLonPoint( -M_PI, averageLatitude, GeoDataCoordinates::Radian );
    
        qreal dummyX, dummyY; // not needed
        bool dummyVal;
    
        if ( screenCoordinates( maxLonPoint, viewport, dummyX, dummyY, dummyVal ) ) {
            latLonAltBox.setEast( +M_PI );
        }
        if ( screenCoordinates( minLonPoint, viewport, dummyX, dummyY, dummyVal ) ) {
            latLonAltBox.setWest( -M_PI );
        }
    }

//    mDebug() << latLonAltBox.text( GeoDataCoordinates::Degree );

    return latLonAltBox;
}


bool MercatorProjection::mapCoversViewport( const ViewportParams *viewport ) const
{
    int           radius = viewport->radius();
    int           height = viewport->height();

    // Calculate translation of center point
    const qreal centerLat = viewport->centerLatitude();

    // Calculate how many pixel are being represented per radians.
    const float rad2Pixel = (float)( 2 * radius )/M_PI;

    int yCenterOffset = (int)( asinh( tan( centerLat ) ) * rad2Pixel  );
    int yTop          = height / 2 - 2 * radius + yCenterOffset;
    int yBottom       = yTop + 4 * radius;

    if ( yTop >= 0 || yBottom < height )
        return false;

    return true;
}

QPainterPath MercatorProjection::mapShape( const ViewportParams *viewport ) const
{
    // Convenience variables
    //int  radius = viewport->radius();
    int  width  = viewport->width();
    int  height = viewport->height();

    qreal  yTop;
    qreal  yBottom;
    qreal  xDummy;
    const AbstractProjection *proj = viewport->currentProjection();

    // Get the top and bottom y coordinates of the projected map.
    proj->screenCoordinates( 0.0, proj->maxLat(), viewport,
                 xDummy, yTop );
    proj->screenCoordinates( 0.0, proj->minLat(), viewport,
                 xDummy, yBottom );

    if ( yTop < 0 )
        yTop = 0;
    if ( yBottom > height )
        yBottom =  height;

    QPainterPath mapShape;
    mapShape.addRect(
                    0,
                    yTop,
                    width,
                    yBottom - yTop );

    return mapShape;
}
