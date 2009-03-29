//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

// Local
#include "AbstractProjection.h"

#include <QtCore/QDebug>

// Marble
#include "ViewportParams.h"

using namespace Marble;

AbstractProjection::AbstractProjection()
{
    m_repeatX = false;
}

AbstractProjection::~AbstractProjection()
{
}


bool AbstractProjection::screenCoordinates( const GeoDataCoordinates &geopoint, 
                                            const ViewportParams *viewport,
                                            int &x, int &y )
{
    bool globeHidesPoint;

    return screenCoordinates( geopoint, viewport, x, y, globeHidesPoint );
}


GeoDataLatLonAltBox AbstractProjection::latLonAltBox( const QRect& screenRect,
                                                      const ViewportParams *viewport )
{
    // For the case where the whole viewport gets covered there is a 
    // pretty dirty and generic detection algorithm:

    int xStep = 4;
    int yStep = 4;

    qreal lon, lat;
    qreal eastLon  = -M_PI; 
    qreal otherEastLon  = -M_PI; 
    qreal westLon  = +M_PI; 
    qreal otherWestLon  = +M_PI; 
    qreal northLat = m_minLat;
    qreal southLat = m_maxLat; 

    // Move along the screenborder and save the highest and lowest lon-lat values.

    for ( int x = screenRect.left(); x < screenRect.right(); x+=xStep ) {
        if ( geoCoordinates( x, screenRect.bottom(), viewport, lon, lat,
                             GeoDataCoordinates::Radian ) ) {
            coordinateExtremes( lon, lat, westLon, eastLon,
                                otherWestLon, otherEastLon,
                                northLat, southLat );
        }

        if ( geoCoordinates( x, screenRect.top(),
                             viewport, lon, lat, GeoDataCoordinates::Radian ) ) {
            coordinateExtremes( lon, lat, westLon, eastLon,
                                otherWestLon, otherEastLon,
                                northLat, southLat );
        }
    }

    if ( geoCoordinates( screenRect.right(), screenRect.top(), viewport, lon, lat,
                         GeoDataCoordinates::Radian ) ) {
        coordinateExtremes( lon, lat, westLon, eastLon,
                            otherWestLon, otherEastLon, northLat, southLat );
    }

    if ( geoCoordinates( screenRect.right(), screenRect.bottom(),
                         viewport, lon, lat, GeoDataCoordinates::Radian ) ) {
        coordinateExtremes( lon, lat, westLon, eastLon,
                            otherWestLon, otherEastLon, northLat, southLat );
    }

    for ( int y = screenRect.bottom(); y < screenRect.top(); y+=yStep ) {
        if ( geoCoordinates( screenRect.left(), y, viewport, lon, lat, 
                             GeoDataCoordinates::Radian ) ) {
            coordinateExtremes( lon, lat, westLon, eastLon,
                                otherWestLon, otherEastLon,
                                northLat, southLat );
        }

        if ( geoCoordinates( screenRect.right(), y,
                             viewport, lon, lat, GeoDataCoordinates::Radian ) ) {
            coordinateExtremes( lon, lat, westLon, eastLon,
                                otherWestLon, otherEastLon,
                                northLat, southLat );
        }
    }

    // DateLine check:
    if ( ( M_PI - eastLon ) < 0.05
         && ( westLon + M_PI ) < 0.05
         && fabs( otherEastLon ) > 0.05
         && fabs( otherWestLon ) > 0.05 )
    {
        westLon = otherWestLon;
        eastLon = otherEastLon;
    }

    // Now we need to check whether maxLat (e.g. the north pole) gets displayed
    // inside the viewport.

    // We need a point on the screen at maxLat that definetely gets displayed:
    qreal averageLongitude = ( westLon + eastLon ) / 2.0;

    GeoDataCoordinates maxLatPoint( averageLongitude, m_maxLat, 0.0, GeoDataCoordinates::Radian );
    GeoDataCoordinates minLatPoint( averageLongitude, m_minLat, 0.0, GeoDataCoordinates::Radian );

    int dummyX, dummyY; // not needed

    if ( screenCoordinates( maxLatPoint, viewport, dummyX, dummyY ) ) {
        northLat = m_maxLat;
    }
    if ( screenCoordinates( minLatPoint, viewport, dummyX, dummyY ) ) {
        southLat = m_minLat;
    }

    GeoDataLatLonAltBox latLonAltBox;
    latLonAltBox.setBoundaries( northLat, southLat, eastLon, westLon, 
                                GeoDataCoordinates::Radian  );

    latLonAltBox.setMinAltitude(      -100000000.0 );
    latLonAltBox.setMaxAltitude( 100000000000000.0 );

    return latLonAltBox;
}

void AbstractProjection::coordinateExtremes( qreal lon, qreal lat,
                                             qreal &westLon, qreal &eastLon,
                                             qreal &otherWestLon, qreal &otherEastLon,
                                             qreal &northLat, qreal &southLat )
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

QPolygonF AbstractProjection::tessellateLineSegment( const GeoDataCoordinates &previousCoords, 
                                                    const GeoDataCoordinates &currentCoords,
                                                    int count,
                                                    const ViewportParams *viewport,
                                                    TessellationFlags f )
{
    QPolygonF   path;

    bool clampToGround = f.testFlag( FollowGround );
    bool followLatitudeCircle = false;     

    if ( count > 50 ) {
//        qDebug() << "Count of" << count << "exceeded maximum count.";
        count = 50;
    }

//    qDebug() << "Creating tesselation nodes:" << count;

    qreal previousAltitude = previousCoords.altitude();

    qreal lonDiff = 0.0;
    qreal previousLongitude = 0.0;
    qreal previousLatitude = 0.0;

    if ( f.testFlag( RespectLatitudeCircle ) ) {
        previousCoords.geoCoordinates( previousLongitude, previousLatitude );

        qreal currentLongitude = 0.0;
        qreal currentLatitude = 0.0;
        currentCoords.geoCoordinates( currentLongitude, currentLatitude );

        if ( previousLatitude == currentLatitude ) {
            followLatitudeCircle = true;
            lonDiff = currentLongitude - previousLongitude;
            if ( fabs( lonDiff ) == 360.0 ) return path; 
        }
        else {
//            qDebug() << "Don't FollowLatitudeCircle";
        }
    }
    else {
//        qDebug() << "Don't RespectLatitudeCircle";
    }
    
    qreal altDiff = currentCoords.altitude() - previousAltitude;

    // Take the clampToGround property into account
    int startNode = clampToGround ? 0 : 0;
    const int endNode = clampToGround ? count + 0 : count + 0;

    qreal  lon = 0.0;
    qreal  lat = 0.0;
    int     x = 0;
    int     y = 0;

    Quaternion  itpos;

    bool globeHidesPoint = false;
    bool previousGlobeHidesPoint = false;
    bool previousVisible = false;

    QPointF point;
    QPointF previousPoint;

    for ( int i = startNode; i <= endNode; ++i ) {
        qreal  t = (qreal)(i) / (qreal)( count ) ;

        // interpolate the altitude, too
        qreal altitude = clampToGround ? 0 : altDiff * t + previousAltitude;

        if ( followLatitudeCircle ) {
            // To tesselate along latitude circles use the 
            // linear interpolation of the longitude.
            lon = lonDiff * t + previousLongitude;
            lat = previousLatitude;
        }
        else {
            // To tesselate along great circles use the 
            // normalized linear interpolation ("NLERP") for latitude and longitude.
            itpos.nlerp( previousCoords.quaternion(), currentCoords.quaternion(), t );

            itpos.getSpherical( lon, lat );
        }

        bool visible = screenCoordinates( GeoDataCoordinates( lon, lat, altitude ), viewport, x, y, globeHidesPoint );

        point = QPointF( x, y );

        // Initialize previous values with current values for the first node.
        if ( i == startNode ) {
            previousPoint = point;
            previousVisible = visible;
            previousGlobeHidesPoint = globeHidesPoint;
        }

        if ( visible && !previousVisible && !previousGlobeHidesPoint ) {
            path << previousPoint;
        }
        // No "else" here, as this would not add the current point that is required.
        if ( ( visible || (!visible && previousVisible ) ) && !globeHidesPoint ) {
            path << point;
        }

        previousPoint = point;
        previousVisible = visible;
        previousGlobeHidesPoint = globeHidesPoint;
    }
    return path; 
}
