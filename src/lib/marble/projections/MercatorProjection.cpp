//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
// Copyright 2007-2012   Torsten Rahn  <rahn@kde.org>
//


// Local
#include "MercatorProjection.h"

#include "MarbleDebug.h"

// Marble
#include "ViewportParams.h"
#include "GeoDataLatLonAltBox.h"

#include "MathHelper.h"
#include "GeoDataPoint.h"
#include "MarbleMath.h"

#include <QIcon>

using namespace Marble;

MercatorProjection::MercatorProjection()
    : CylindricalProjection(),
      m_lastCenterLat(200.0),
      m_lastCenterLatInv(0.0)
{
    setMinLat( minValidLat() );
    setMaxLat( maxValidLat() );
}

MercatorProjection::~MercatorProjection()
{
}

QString MercatorProjection::name() const
{
    return QObject::tr( "Mercator" );
}

QString MercatorProjection::description() const
{
    return QObject::tr( "<p><b>Mercator Projection</b></p><p>Applications: popular standard map projection for navigation.</p>" );
}

QIcon MercatorProjection::icon() const
{
    return QIcon(QStringLiteral(":/icons/map-mercator.png"));
}

qreal MercatorProjection::maxValidLat() const
{
    // This is the max value where gd( lat ) is defined.
    return +85.05113 * DEG2RAD;
}

qreal MercatorProjection::minValidLat() const
{
    // This is the min value where gd( lat ) is defined.
    return -85.05113 * DEG2RAD;
}

bool MercatorProjection::screenCoordinates( const GeoDataCoordinates &geopoint, 
                                            const ViewportParams *viewport,
                                            qreal &x, qreal &y, bool &globeHidesPoint ) const
{
    globeHidesPoint = false;
    qreal  lon;
    qreal  originalLat;

    geopoint.geoCoordinates( lon, originalLat );
    qreal const lat = qBound(minLat(), originalLat, maxLat());
    const bool isLatValid = lat == originalLat;

    // Convenience variables
    int  radius = viewport->radius();
    qreal  width  = (qreal)(viewport->width());
    qreal  height = (qreal)(viewport->height());

    qreal  rad2Pixel = 2 * radius / M_PI;

    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();
    if (centerLat != m_lastCenterLat) {
        m_lastCenterLatInv = gdInv(centerLat);
        m_lastCenterLat = centerLat;
    }

    // Let (x, y) be the position on the screen of the placemark..
    x = ( width  / 2 + rad2Pixel * ( lon - centerLon ) );
    y = ( height / 2 - rad2Pixel * ( gdInv( lat ) - m_lastCenterLatInv ) );

    // Return true if the calculated point is inside the screen area,
    // otherwise return false.
    return isLatValid && ( ( 0 <= y && y < height )
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
    pointRepeatNum = 0;
    // On flat projections the observer's view onto the point won't be 
    // obscured by the target planet itself.
    globeHidesPoint = false;

    // Convenience variables
    int  radius = viewport->radius();
    qreal  width  = (qreal)(viewport->width());
    qreal  height = (qreal)(viewport->height());

    // Let (itX, y) be the first guess for one possible position on screen..
    qreal itX;
    bool visible = screenCoordinates( coordinates, viewport, itX, y);

    // Make sure that the requested point is within the visible y range:
    if ( 0 <= y + size.height() / 2.0 && y < height + size.height() / 2.0 ) {
        // For the repetition case the same geopoint gets displayed on 
        // the map many times.across the longitude.

        int xRepeatDistance = 4 * radius;

        // Finding the leftmost positive x value
        if ( itX + size.width() / 2.0 >= xRepeatDistance ) {
            const int repeatNum = (int)( ( itX + size.width() / 2.0 ) / xRepeatDistance );
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

        return visible;
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
    Q_ASSERT( radius > 0 );

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
            lat = gd( ( ( halfImageHeight + yCenterOffset ) - y)
                              * pixel2Rad );

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

    int xRepeatDistance = 4 * viewport->radius();
    if ( viewport->width() >= xRepeatDistance ) {
        latLonAltBox.setWest( -M_PI );
        latLonAltBox.setEast( +M_PI );
    }

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

    return !(yTop >= 0 || yBottom < height);
}
