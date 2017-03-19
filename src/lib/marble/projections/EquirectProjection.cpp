//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007-2012   Torsten Rahn  <rahn@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
//


// Local
#include "EquirectProjection.h"

// Marble
#include "ViewportParams.h"
#include "GeoDataLatLonAltBox.h"

#include "MarbleDebug.h"

#include <QIcon>

using namespace Marble;


EquirectProjection::EquirectProjection()
    : CylindricalProjection()
{
    setMinLat( minValidLat() );
    setMaxLat( maxValidLat() );
}

EquirectProjection::~EquirectProjection()
{
}

QString EquirectProjection::name() const
{
    return QObject::tr( "Flat Map" );
}

QString EquirectProjection::description() const
{
    return QObject::tr( "<p><b>Equirectangular Projection</b> (\"Plate carrée\")</p><p>Applications: De facto standard for global texture data sets for computer software.</p>" );
}

QIcon EquirectProjection::icon() const
{
    return QIcon(QStringLiteral(":/icons/map-flat.png"));
}

qreal EquirectProjection::maxValidLat() const
{
    return +90.0 * DEG2RAD;
}

qreal EquirectProjection::minValidLat() const
{
    return -90.0 * DEG2RAD;
}

bool EquirectProjection::screenCoordinates( const GeoDataCoordinates &geopoint, 
                                            const ViewportParams *viewport,
                                            qreal &x, qreal &y, bool &globeHidesPoint ) const
{
    globeHidesPoint = false;

    // Convenience variables
    int  radius = viewport->radius();
    int  width  = viewport->width();
    int  height = viewport->height();

    qreal  lon;
    qreal  lat;
    qreal  rad2Pixel = 2.0 * viewport->radius() / M_PI;

    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();

    geopoint.geoCoordinates( lon, lat );

    // Let (x, y) be the position on the screen of the geopoint.
    x = ((qreal)(viewport->width())  / 2.0 + rad2Pixel * (lon - centerLon));
    y = ((qreal)(viewport->height()) / 2.0 - rad2Pixel * (lat - centerLat));

    // Return true if the calculated point is inside the screen area,
    // otherwise return false.
    return ( ( 0 <= y && y < height )
             && ( ( 0 <= x && x < width )
                  || ( 0 <= x - 4 * radius && x - 4 * radius < width )
                  || ( 0 <= x + 4 * radius && x + 4 * radius < width ) ) );
}

bool EquirectProjection::screenCoordinates( const GeoDataCoordinates &coordinates,
                                            const ViewportParams *viewport,
                                            qreal *x, qreal &y,
                                            int &pointRepeatNum,
                                            const QSizeF& size,
                                            bool &globeHidesPoint ) const
{
    pointRepeatNum = 0;
    // On flat projections the observer's view onto the point won't be
    // obscured by the target planet itself.
    globeHidesPoint = false;

    // Convenience variables
    int  radius = viewport->radius();
    qreal  width  = (qreal)(viewport->width());
    qreal  height = (qreal)(viewport->height());

    // Let (itX, y) be the first guess for one possible position on screen.
    qreal itX;
    screenCoordinates( coordinates, viewport, itX, y);

    // Make sure that the requested point is within the visible y range:
    if ( 0 <= y + size.height() / 2.0 && y < height + size.height() / 2.0 ) {
        // For the repetition case the same geopoint gets displayed on 
        // the map many times.across the longitude.

        int xRepeatDistance = 4 * radius;

        // Finding the leftmost positive x value
        if ( itX + size.width() > xRepeatDistance ) {
            const int repeatNum = (int)( ( itX + size.width() ) / xRepeatDistance );
            itX = itX - repeatNum * xRepeatDistance;
        }
        if ( itX + size.width() / 2.0 < 0 ) {
            itX += xRepeatDistance;
        }
        // The requested point is out of the visible x range:
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

        return true;
    }

    // The requested point is out of the visible y range.
    return false;
}


bool EquirectProjection::geoCoordinates( const int x, const int y,
                                         const ViewportParams *viewport,
                                         qreal& lon, qreal& lat,
                                         GeoDataCoordinates::Unit unit ) const
{
    const int radius = viewport->radius();
    const qreal pixel2Rad = M_PI / (2.0 * radius);

    // Get the Lat and Lon of the center point of the screen.
    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();

    {
        const int halfImageWidth = viewport->width() / 2;
        const int xPixels = x - halfImageWidth;

        lon = + xPixels * pixel2Rad + centerLon;

        while ( lon > M_PI )  lon -= 2.0 * M_PI;
        while ( lon < -M_PI ) lon += 2.0 * M_PI;

        if ( unit == GeoDataCoordinates::Degree ) {
            lon *= RAD2DEG;
        }
    }

    {
        // Get yTop and yBottom, the limits of the map on the screen.
        const int halfImageHeight = viewport->height() / 2;
        const int yCenterOffset = (int)( centerLat * (qreal)(2 * radius) / M_PI);
        const int yTop          = halfImageHeight - radius + yCenterOffset;
        const int yBottom       = yTop + 2 * radius;

        // Return here if the y coordinate is outside the map
        if ( yTop <= y && y < yBottom ) {
            const int yPixels = y - halfImageHeight;
            lat = - yPixels * pixel2Rad + centerLat;

            if ( unit == GeoDataCoordinates::Degree ) {
                lat *= RAD2DEG;
            }

            return true;
        }
    }

    return false;
}

GeoDataLatLonAltBox EquirectProjection::latLonAltBox( const QRect& screenRect,
                                                      const ViewportParams *viewport ) const
{
    qreal west;
    qreal north = 90*DEG2RAD;
    geoCoordinates( screenRect.left(), screenRect.top(), viewport, west, north, GeoDataCoordinates::Radian );

    qreal east;
    qreal south = -90*DEG2RAD;
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

    // Convenience variables
    int  radius = viewport->radius();
    int  width  = viewport->width();

    // The remaining algorithm should be pretty generic for all kinds of 
    // flat projections:

    int xRepeatDistance = 4 * radius;
    if ( width >= xRepeatDistance ) {
        latLonAltBox.setWest( -M_PI );
        latLonAltBox.setEast( +M_PI );
    }

    // Now we need to check whether maxLat (e.g. the north pole) gets displayed
    // inside the viewport.

    // We need a point on the screen at maxLat that definitely gets displayed:
    qreal averageLongitude = latLonAltBox.east();

    GeoDataCoordinates maxLatPoint( averageLongitude, maxLat(), 0.0, GeoDataCoordinates::Radian );
    GeoDataCoordinates minLatPoint( averageLongitude, minLat(), 0.0, GeoDataCoordinates::Radian );

    qreal dummyX, dummyY; // not needed

    if ( screenCoordinates( maxLatPoint, viewport, dummyX, dummyY ) ) {
        latLonAltBox.setEast( +M_PI );
        latLonAltBox.setWest( -M_PI );
    }
    if ( screenCoordinates( minLatPoint, viewport, dummyX, dummyY ) ) {
        latLonAltBox.setEast( +M_PI );
        latLonAltBox.setWest( -M_PI );
    }

    return latLonAltBox;
}


bool EquirectProjection::mapCoversViewport( const ViewportParams *viewport ) const
{
    // Convenience variables
    int  radius          = viewport->radius();
    //int  width         = viewport->width();
    int  height          = viewport->height();
    int  halfImageHeight = viewport->height() / 2;

    // Get the Lat and Lon of the center point of the screen.
    const qreal centerLat = viewport->centerLatitude();

    // Calculate how many pixel are being represented per radians.
    const float rad2Pixel = (qreal)( 2 * radius )/M_PI;

    // Get yTop and yBottom, the limits of the map on the screen.
    int yCenterOffset = (int)( centerLat * rad2Pixel );
    int yTop          = halfImageHeight - radius + yCenterOffset;
    int yBottom       = yTop + 2 * radius;

    return !(yTop >= 0 || yBottom < height);
}
