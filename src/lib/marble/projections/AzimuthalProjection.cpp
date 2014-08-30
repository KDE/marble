//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014       Gábor Péterffy  <peterffy95@gmail.org>
//

// Local
#include "AzimuthalProjection.h"
#include "AzimuthalProjection_p.h"
#include "AbstractProjection_p.h"

// Marble
#include "GeoDataLinearRing.h"
#include "GeoDataLineString.h"
#include "GeoDataCoordinates.h"
#include "ViewportParams.h"

namespace Marble {

qreal AzimuthalProjection::maxValidLat() const
{
    return +90.0 * DEG2RAD;
}

qreal AzimuthalProjection::minValidLat() const
{
    return -90.0 * DEG2RAD;
}

GeoDataLatLonAltBox AzimuthalProjection::latLonAltBox( const QRect& screenRect,
                                                       const ViewportParams *viewport ) const
{
    // For the case where the whole viewport gets covered there is a
    // pretty dirty and generic detection algorithm:
    GeoDataLatLonAltBox latLonAltBox = AbstractProjection::latLonAltBox( screenRect, viewport );

    // If the whole globe is visible we can easily calculate
    // analytically the lon-/lat- range.
    qreal pitch = GeoDataCoordinates::normalizeLat( viewport->planetAxis().pitch() );

    if ( 2.0 * viewport->radius() <= viewport->height()
         &&  2.0 * viewport->radius() <= viewport->width() )
    {
        // Unless the planetaxis is in the screen plane the allowed longitude range
        // covers full -180 deg to +180 deg:
        if ( pitch > 0.0 && pitch < +M_PI ) {
            latLonAltBox.setNorth( +fabs( M_PI / 2.0 - fabs( pitch ) ) );
            latLonAltBox.setSouth( -M_PI / 2.0 );
        }
        if ( pitch < 0.0 && pitch > -M_PI ) {
            latLonAltBox.setWest(  -M_PI );
            latLonAltBox.setEast(  +M_PI );
            latLonAltBox.setNorth( +M_PI / 2.0 );
            latLonAltBox.setSouth( -fabs( M_PI / 2.0 - fabs( pitch ) ) );
        }

        // Last but not least we deal with the rare case where the
        // globe is fully visible and pitch = 0.0 or pitch = -M_PI or
        // pitch = +M_PI
        if ( pitch == 0.0 || pitch == -M_PI || pitch == +M_PI ) {
            qreal yaw = viewport->planetAxis().yaw();
            latLonAltBox.setWest( GeoDataCoordinates::normalizeLon( yaw - M_PI / 2.0 ) );
            latLonAltBox.setEast( GeoDataCoordinates::normalizeLon( yaw + M_PI / 2.0 ) );
            latLonAltBox.setNorth( +M_PI / 2.0 );
            latLonAltBox.setSouth( -M_PI / 2.0 );
        }
    }

    // Now we check whether maxLat (e.g. the north pole) gets displayed
    // inside the viewport to get more accurate values for east and west.

    // We need a point on the screen at maxLat that definitely gets displayed:
    qreal averageLongitude = ( latLonAltBox.west() + latLonAltBox.east() ) / 2.0;

    GeoDataCoordinates maxLatPoint( averageLongitude, maxLat(), 0.0, GeoDataCoordinates::Radian );
    GeoDataCoordinates minLatPoint( averageLongitude, minLat(), 0.0, GeoDataCoordinates::Radian );

    qreal dummyX, dummyY; // not needed
    bool dummyVal;

    if ( screenCoordinates( maxLatPoint, viewport, dummyX, dummyY, dummyVal ) ||
         screenCoordinates( minLatPoint, viewport, dummyX, dummyY, dummyVal ) ) {
        latLonAltBox.setWest( -M_PI );
        latLonAltBox.setEast( +M_PI );
    }

    return latLonAltBox;
}

QPainterPath AzimuthalProjection::mapShape( const ViewportParams *viewport ) const
{
    int  radius    = viewport->radius();
    int  imgWidth  = viewport->width();
    int  imgHeight = viewport->height();

    QPainterPath fullRect;
    fullRect.addRect(  0 , 0 , imgWidth, imgHeight );

    // If the globe covers the whole image, then the projected region represents
    // all of the image.
    // Otherwise the active region has got the shape of the visible globe.

    if ( !viewport->mapCoversViewport() ) {
        QPainterPath mapShape;
        mapShape.addEllipse(
            imgWidth  / 2 - radius,
            imgHeight / 2 - radius,
            2 * radius,
            2 * radius );
        return mapShape.intersected( fullRect );
    }

    return fullRect;
}

AzimuthalProjection::AzimuthalProjection(AzimuthalProjectionPrivate * dd) :
    AbstractProjection( dd )
{
}

AzimuthalProjection::~AzimuthalProjection()
{
}

}


