//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007        Inge Wallin   <ingwa@kde.org>
// Copyright 2007-2009   Torsten Rahn  <rahn@kde.org>
//

// Local
#include "AbstractProjection.h"

#include <QtCore/QDebug>

// Marble
#include "ViewportParams.h"

using namespace Marble;

// Maximum amount of nodes that are created automatically between actual nodes.
static const int maxTessellationNodes = 200;

AbstractProjection::AbstractProjection()
{
    m_repeatX = false;
}

AbstractProjection::~AbstractProjection()
{
}

bool AbstractProjection::screenCoordinates( const GeoDataCoordinates &geopoint, 
                                            const ViewportParams *viewport,
                                            qreal &x, qreal &y )
{
    bool globeHidesPoint;

    return screenCoordinates( geopoint, viewport, x, y, globeHidesPoint );
}

bool AbstractProjection::screenCoordinates( const Marble::GeoDataCoordinates &coordinates,
                                    const ViewportParams *viewport,
                                    qreal *x, qreal &y, int &pointRepeatNum,                                    bool &globeHidesPoint )
{
    return screenCoordinates( coordinates, viewport, x, y, pointRepeatNum, 
           QSizeF( 0.0, 0.0 ), globeHidesPoint );
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

    qreal dummyX, dummyY; // not needed

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

bool AbstractProjection::exceedsLatitudeRange( const GeoDataLineString &lineString ) const
{
    GeoDataLatLonAltBox latLonAltBox = lineString.latLonAltBox();

    return (    latLonAltBox.north() >= m_maxLat
             || latLonAltBox.south() <= m_minLat );
}


bool AbstractProjection::exceedsLatitudeRange( const GeoDataCoordinates &coords ) const
{
    qreal lat = coords.latitude();
    // Evaluate the most likely case first:
    // The case where we are within the range and where our latitude is normalized
    // to the range of 90 deg S ... 90 deg N
    if ( lat < m_maxLat && lat > m_minLat ) {
        return false;
    }
    else {
        // If we are not within the range then normalize the latitude and check again.
        qreal normalizedLat = GeoDataCoordinates::normalizeLat( lat );
        if ( normalizedLat == lat ) {
            return true;
        }

        if ( normalizedLat < m_maxLat && normalizedLat > m_minLat ) {
            // FIXME: Should we just normalize latitude and longitude and be done?
            //        While this might work well for persistent data it would create some 
            //        possible overhead for temporary data, so this needs careful thinking.
            qDebug() << "GeoDataCoordinates not normalized!";
            return false;
        }
    }

    // We have exceeded the range.
    return true;
}

QPolygonF AbstractProjection::tessellateLineSegment( const GeoDataCoordinates &previousCoords, 
                                                    const GeoDataCoordinates &currentCoords,
                                                    int tessellatedNodes,
                                                    const ViewportParams *viewport,
                                                    TessellationFlags f )
{
    QPolygonF   path;

    const bool clampToGround = f.testFlag( FollowGround );
    bool followLatitudeCircle = false;     

    // Maximum amount of tesselation nodes.
    if ( tessellatedNodes > maxTessellationNodes ) tessellatedNodes = maxTessellationNodes;

//    qDebug() << "Creating tesselation nodes:" << tessellatedNodes;

    qreal previousAltitude = previousCoords.altitude();

    // Calculate steps for tessellation: lonDiff and altDiff 
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
            if ( fabs( lonDiff ) == 2 * M_PI ) {
                return path;
            }
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
    int startNode = 1;
    const int endNode = tessellatedNodes - 2;

    qreal  lon = 0.0;
    qreal  lat = 0.0;
    qreal     x = 0;
    qreal     y = 0;
    Quaternion  itpos;

    // Declare current values.
    QPointF point;
    bool globeHidesPoint = false;

    // For the clampToGround case add the "previous" coordinate before adding any other node. 
    if ( clampToGround && previousAltitude != 0.0 ) {
          bool visible = screenCoordinates( previousCoords, viewport, x, y, globeHidesPoint );
          if ( !globeHidesPoint ) {
            path << QPointF( x, y );
          }
    }

    GeoDataCoordinates previousModifiedCoords( previousCoords );
    if ( clampToGround ) {
        previousModifiedCoords.setAltitude( 0.0 );
    }
    bool previousVisible = screenCoordinates( previousModifiedCoords, viewport, x, y, globeHidesPoint );
    if ( !globeHidesPoint ) {
        path << QPointF( x, y );
    }

    // Create the tessellation nodes.
    for ( int i = startNode; i <= endNode; ++i ) {
        qreal  t = (qreal)(i) / (qreal)( tessellatedNodes ) ;

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
            itpos. getSpherical( lon, lat );
        }

        screenCoordinates( GeoDataCoordinates( lon, lat, altitude ), viewport, x, y, globeHidesPoint );
        point = QPointF( x, y );

        // No "else" here, as this would not add the current point that is required.
        if ( !globeHidesPoint ) {
            path << point;
        }
    }


    GeoDataCoordinates currentModifiedCoords( currentCoords );
    if ( clampToGround ) {
        currentModifiedCoords.setAltitude( 0.0 );
    }
    bool currentVisible = screenCoordinates( currentModifiedCoords, viewport, x, y, globeHidesPoint );
    if ( !globeHidesPoint ) {
        path << QPointF( x, y );
    }

    // For the clampToGround case add the "current" coordinate after adding all other nodes. 
    if ( clampToGround && currentCoords.altitude() != 0.0 ) {
          bool visible = screenCoordinates( currentCoords, viewport, x, y, globeHidesPoint );
          if ( !globeHidesPoint ) {
            path << QPointF( x, y );
          }
    }

    return path; 
}

/* DEPRECATED */
bool AbstractProjection::screenCoordinates( const GeoDataCoordinates &geopoint, 
                                            const ViewportParams *viewport,
                                            int &x, int &y, bool &globeHidesPoint )
{
    qreal rx = 0.0;
    qreal ry = 0.0;    

    bool isVisible = screenCoordinates( geopoint, viewport, rx, ry, globeHidesPoint );

    x = (int)(rx); y = (int)(ry);

    return isVisible;
}

/* DEPRECATED */
bool AbstractProjection::screenCoordinates( qreal lon, qreal lat,
                                    const ViewportParams *viewport,
                                    int &x, int &y )
{
    GeoDataCoordinates geopoint( lon, lat );

    bool globeHidesPoint;
    bool isVisible = screenCoordinates( geopoint, viewport, x, y, globeHidesPoint );

    return isVisible;
}
