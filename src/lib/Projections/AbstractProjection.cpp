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
#include "AbstractProjection.h"

// Marble
#include "ViewportParams.h"

AbstractProjection::AbstractProjection()
{
    m_repeatX = false;
}

AbstractProjection::~AbstractProjection()
{
}

bool AbstractProjection::screenCoordinates( const GeoDataPoint &geopoint, 
                         const ViewportParams *viewport, int &x, int &y )
{
    bool globeHidesPoint;
    return screenCoordinates( geopoint, viewport, x, y, globeHidesPoint );
}

GeoDataLatLonAltBox AbstractProjection::latLonAltBox( const QRect& screenRect, const ViewportParams *viewport )
{
    // For the case where the whole viewport gets covered there is a 
    // pretty dirty and generic detection algorithm:

    int xStep = 4;
    int yStep = 4;

    double lon, lat;
    double eastLon  = -M_PI; 
    double otherEastLon  = -M_PI; 
    double westLon  = +M_PI; 
    double otherWestLon  = +M_PI; 
    double northLat = -m_maxLat; 
    double southLat = +m_maxLat; 

    // Move along the screenborder and save the highest and lowest lon-lat values.

    for ( int x = 0; x < viewport->width(); x+=xStep )
    {
        if ( geoCoordinates( x, 0, viewport, lon, lat, GeoDataPoint::Radian ) ) {
            coordinateExtremes( lon, lat, westLon, eastLon, otherWestLon, otherEastLon, northLat, southLat );
        }

        if ( geoCoordinates( x, viewport->height() - 1, viewport, lon, lat, GeoDataPoint::Radian ) ) {
            coordinateExtremes( lon, lat, westLon, eastLon, otherWestLon, otherEastLon, northLat, southLat );
        }
    }

    if ( geoCoordinates( viewport->width(), 0, viewport, lon, lat, GeoDataPoint::Radian ) ) {
        coordinateExtremes( lon, lat, westLon, eastLon, otherWestLon, otherEastLon, northLat, southLat );
    }

    if ( geoCoordinates( viewport->width(), viewport->height() - 1, viewport, lon, lat, GeoDataPoint::Radian ) ) {
        coordinateExtremes( lon, lat, westLon, eastLon, otherWestLon, otherEastLon, northLat, southLat );
    }

    for ( int y = 0; y < viewport->height(); y+=yStep )
    {
        if ( geoCoordinates( 0, y, viewport, lon, lat, GeoDataPoint::Radian ) ) {
            coordinateExtremes( lon, lat, westLon, eastLon, otherWestLon, otherEastLon, northLat, southLat );
        }

        if ( geoCoordinates( viewport->width() - 1, y, viewport, lon, lat, GeoDataPoint::Radian ) ) {
            coordinateExtremes( lon, lat, westLon, eastLon, otherWestLon, otherEastLon, northLat, southLat );
        }
    }

    // DateLine check:

    if ( ( M_PI - eastLon ) < 0.05 && ( westLon + M_PI ) < 0.05 && fabs( otherEastLon ) > 0.05 && fabs( otherWestLon ) > 0.05 ) {
        westLon = otherWestLon;
        eastLon = otherEastLon;
    }

    // Now we need to check whether maxLat (e.g. the north pole) gets displayed
    // inside the viewport.

    // We need a point on the screen at maxLat that definetely gets displayed:
    double averageLongitude = ( westLon + eastLon ) / 2.0;

    GeoDataPoint maxLatPoint( averageLongitude, +m_maxLat, 0.0, GeoDataPoint::Radian );
    GeoDataPoint minLatPoint( averageLongitude, -m_maxLat, 0.0, GeoDataPoint::Radian );

    int dummyX, dummyY; // not needed

    if ( screenCoordinates( maxLatPoint, viewport, dummyX, dummyY ) ) {
        northLat = +m_maxLat;
    }
    if ( screenCoordinates( minLatPoint, viewport, dummyX, dummyY ) ) {
        southLat = -m_maxLat;
    }

    GeoDataLatLonAltBox latLonAltBox;
    latLonAltBox.setBoundaries( westLon, eastLon, northLat, southLat, GeoDataPoint::Radian  );

    latLonAltBox.setMinAltitude(      -100000000.0 );
    latLonAltBox.setMaxAltitude( 100000000000000.0 );

    return latLonAltBox;
}

void AbstractProjection::coordinateExtremes( double lon, double lat,
                                             double &westLon, double &eastLon,
                                             double &otherWestLon, double &otherEastLon,
                                             double &northLat, double &southLat )
{
    if ( lon < westLon ) westLon = lon;
    if ( lon < otherWestLon && lon > 0.0 ) otherWestLon = lon;
    if ( lon > eastLon ) eastLon = lon;
    if ( lon > otherEastLon && lon < 0.0 ) otherEastLon = lon;
    if ( lat > northLat ) northLat = lat;
    if ( lat < southLat ) southLat = lat;
}

GeoDataLinearRing AbstractProjection::rectOutline( const QRect& screenRect,
                                 const ViewportParams *viewport )
{
    return GeoDataLinearRing();
}
