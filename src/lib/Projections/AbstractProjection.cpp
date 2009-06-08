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
#include <QtGui/QRegion>

// Marble
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
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

bool AbstractProjection::screenCoordinates( const GeoDataLineString &lineString,
                                                  const ViewportParams *viewport,
                                                  QVector<QPolygonF *> &polygons )
{
    // Compare bounding box size of the line string with the angularResolution
    // Immediately return if the latLonAltBox is smaller.
    if ( !viewport->resolves( lineString.latLonAltBox() ) ) {
//      qDebug() << "Object too small to be resolved";
        return false;
    }

    QVector<GeoDataLineString> lineStrings;

    if (
         ( !traversablePoles() && lineString.latLonAltBox().containsPole( Marble::AnyPole ) ) ||
         ( !traversableDateLine() && lineString.latLonAltBox().crossesDateLine() )
       ) {
        // We correct for Poles and DateLines:
        lineStrings = lineString.toRangeCorrected();
    }
    else {
        lineStrings << lineString;
    }

    foreach ( const GeoDataLineString & itLineString, lineStrings ) {
        QVector<QPolygonF *> subPolygons;
        lineStringToPolygon( itLineString, viewport, subPolygons );
        polygons << subPolygons;
    }

    return polygons.isEmpty();
}

bool AbstractProjection::lineStringToPolygon( const GeoDataLineString &lineString,
                                                  const ViewportParams *viewport,
                                                  QVector<QPolygonF *> &polygons )
{
    const TessellationFlags tessellationFlags = lineString.tessellationFlags();

    qreal x = 0;
    qreal y = 0;
    bool globeHidesPoint = false;

    qreal previousX = -1.0;
    qreal previousY = -1.0;
    bool previousGlobeHidesPoint = false;

    QPolygonF  *polygon = new QPolygonF;

    GeoDataLineString::ConstIterator itCoords = lineString.constBegin();
    GeoDataLineString::ConstIterator itPreviousCoords = lineString.constBegin();

    GeoDataCoordinates previousCoords;
    GeoDataCoordinates currentCoords;

    GeoDataLineString::ConstIterator itBegin = lineString.constBegin();
    GeoDataLineString::ConstIterator itEnd = lineString.constEnd();

    bool processingLastNode = false;

    // We use a while loop to be able to cover linestrings as well as linear rings:
    // Linear rings require to tessellate the path from the last node to the first node
    // which isn't really convenient to achieve with a for loop ...

    const bool isLong = lineString.size() > 50;

    while ( itCoords != itEnd )
    {
        // Optimization for line strings with a big amount of nodes
        bool skipNode = isLong && viewport->resolves( *itPreviousCoords, *itCoords);

        if ( !skipNode ) {

            previousCoords = *itPreviousCoords;
            currentCoords  = *itCoords;

            screenCoordinates( currentCoords, viewport, x, y, globeHidesPoint );

            // Initializing variables that store the values of the previous iteration
            if ( !processingLastNode && itCoords == itBegin ) {
                previousGlobeHidesPoint = globeHidesPoint;
                itPreviousCoords = itCoords;
                previousX = x;
                previousY = y;
            }

            // Check for the "horizon case" (which is present e.g. for the spherical projection
            if ( globeHidesPoint || previousGlobeHidesPoint ) {
                if ( globeHidesPoint !=  previousGlobeHidesPoint ) {

                    // Handle the "horizon case"
                    tessellateHorizon( previousCoords, currentCoords, viewport );
                }
                else {
                    // Both nodes are located on the planet's hemisphere that is
                    // not visible to the user.
                }
            }

            // This if-clause contains the section that tessellates the line
            // segments of a linestring. If you are about to learn how the code of
            // this class works you can safely ignore this section for a start.

            if ( lineString.tessellate() /* && ( isVisible || previousIsVisible ) */ ) {
                // Let the line segment follow the spherical surface
                // if the distance between the previous point and the current point
                // on screen is too big

                // We take the manhattan length as a distance approximation
                // that can be too big by a factor of sqrt(2)
                qreal distance = fabs((x - previousX)) + fabs((y - previousY));

                // FIXME: This is a work around: remove as soon as we handle horizon crossing
                if ( globeHidesPoint || previousGlobeHidesPoint ) {
                    distance = 350;
                }

#ifdef SAFE_DISTANCE
                // Interpolate additional nodes if the line segment that connects the
                // current or previous nodes might cross the viewport.
                // The latter can pretty safely be excluded for most projections if both points
                // are located on the same side relative to the viewport boundaries and if they are
                // located more than half the line segment distance away from the viewport.
                const qreal safeDistance = - 0.5 * distance;
                if (   !( x < safeDistance && previousX < safeDistance )
                    || !( y < safeDistance && previousY < safeDistance )
                    || !( x + safeDistance > viewport->width()
                        && previousX + safeDistance > viewport->width() )
                    || !( y + safeDistance > viewport->height()
                        && previousY + safeDistance > viewport->height() )
                )
                {
#endif
                    int tessellatedNodes = (int)( distance / tessellationPrecision );

                    if ( distance > tessellationPrecision ) {
//                      qDebug() << "Distance: " << distance;
                        *polygon << tessellateLineSegment( previousCoords, currentCoords,
                                                        tessellatedNodes, viewport,
                                                        tessellationFlags );
                    }
#ifdef SAFE_DISTANCE
                }
#endif
            }
            else {
                if ( !globeHidesPoint ) {
                    polygon->append( QPointF( x, y ) );
                }
            }

            if ( globeHidesPoint ) {
                if (   !previousGlobeHidesPoint
                    && !lineString.isClosed() // FIXME: this probably needs to take rotation
                                                //        into account for some cases
                    ) {
                    polygons.append( polygon );
                    polygon = new QPolygonF;
                }
            }

            previousGlobeHidesPoint = globeHidesPoint;
            itPreviousCoords = itCoords;
            previousX = x;
            previousY = y;
        }

        // Here we modify the condition to be able to process the
        // first node after the last node in a LinearRing.

        if ( processingLastNode ) {
            break;
        }
        ++itCoords;

        if ( itCoords == itEnd  && lineString.isClosed() ) {
            itCoords = itBegin;
            processingLastNode = true;
        }
    }

    if ( polygon->size() > 1 ){
        polygons.append( polygon );
    }
    else {
        delete polygon;
    }

    return polygons.isEmpty();
}

void AbstractProjection::tessellateHorizon( const GeoDataCoordinates &previousCoords,
                                            const GeoDataCoordinates &currentCoords,
                                            const ViewportParams *viewport )
{
/*
    // the line string disappears behind the visible horizon or
    // it reappears at the horizon.
    // Add interpolated "horizon" nodes

    // Assign the first or last horizon point to the current or
    // previous point, so that we still get correct results for
    // the case where we need to geoproject the line segment.
    if ( globeHidesPoint ) {
        currentCoords  = createHorizonCoordinates( previousCoords,
                                                    currentCoords,
                                                    viewport, lineString.tessellationFlags() );
    } else {
        previousCoords = createHorizonCoordinates( previousCoords,
                                                    currentCoords,
                                                    viewport, lineString.tessellationFlags() );
    }
*/
}

GeoDataLatLonAltBox AbstractProjection::latLonAltBox( const QRect& screenRect,
                                                      const ViewportParams *viewport )
{
    // For the case where the whole viewport gets covered there is a 
    // pretty dirty and generic detection algorithm:

    // Move along the screenborder and save the highest and lowest lon-lat values.
    QRect projectedRect = mapRegion( viewport ).boundingRect();
    QRect mapRect = screenRect.intersected( projectedRect );

    GeoDataLineString boundingLineString;

    qreal lon, lat;

    for ( int x = mapRect.left(); x < mapRect.right(); x += latLonAltBoxSamplingRate ) {
        if ( geoCoordinates( x, mapRect.bottom(), viewport, lon, lat,
                             GeoDataCoordinates::Radian ) ) {
            boundingLineString << GeoDataCoordinates( lon, lat );
        }

        if ( geoCoordinates( x, mapRect.top(),
                             viewport, lon, lat, GeoDataCoordinates::Radian ) ) {
            boundingLineString << GeoDataCoordinates( lon, lat );
        }
    }

    if ( geoCoordinates( mapRect.right(), mapRect.top(), viewport, lon, lat,
                         GeoDataCoordinates::Radian ) ) {
        boundingLineString << GeoDataCoordinates( lon, lat );
    }

    if ( geoCoordinates( mapRect.right(), mapRect.bottom(),
                         viewport, lon, lat, GeoDataCoordinates::Radian ) ) {
        boundingLineString << GeoDataCoordinates( lon, lat );
    }

    for ( int y = mapRect.bottom(); y < mapRect.top(); y += latLonAltBoxSamplingRate ) {
        if ( geoCoordinates( mapRect.left(), y, viewport, lon, lat,
                             GeoDataCoordinates::Radian ) ) {
            boundingLineString << GeoDataCoordinates( lon, lat );
        }

        if ( geoCoordinates( mapRect.right(), y,
                             viewport, lon, lat, GeoDataCoordinates::Radian ) ) {
            boundingLineString << GeoDataCoordinates( lon, lat );
        }
    }

    GeoDataLatLonAltBox latLonAltBox = boundingLineString.latLonAltBox();
    
    // Now we need to check whether maxLat (e.g. the north pole) gets displayed
    // inside the viewport.

    // We need a point on the screen at maxLat that definetely gets displayed:

    // FIXME: Some of the following code can be safely removed as soon as we properly handle
    //        GeoDataLinearRing::latLonAltBox().
    qreal averageLongitude = ( latLonAltBox.west() + latLonAltBox.east() ) / 2.0;

    GeoDataCoordinates maxLatPoint( averageLongitude, m_maxLat, 0.0, GeoDataCoordinates::Radian );
    GeoDataCoordinates minLatPoint( averageLongitude, m_minLat, 0.0, GeoDataCoordinates::Radian );

    qreal dummyX, dummyY; // not needed

    if ( latLonAltBox.north() > m_maxLat ||
         screenCoordinates( maxLatPoint, viewport, dummyX, dummyY ) ) {
        latLonAltBox.setNorth( m_maxLat );
    }
    if ( latLonAltBox.north() < m_minLat ||
         screenCoordinates( minLatPoint, viewport, dummyX, dummyY ) ) {
        latLonAltBox.setSouth( m_minLat );
    }

    latLonAltBox.setMinAltitude(      -100000000.0 );
    latLonAltBox.setMaxAltitude( 100000000000000.0 );

    return latLonAltBox;
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
    

    qreal     x = 0;
    qreal     y = 0;

    // Declare current values.
    bool globeHidesPoint = false;

    // Take the clampToGround property into account
    // For the clampToGround case add the "previous" coordinate before adding any other node. 
    if ( clampToGround && previousAltitude != 0.0 ) {
          screenCoordinates( previousCoords, viewport, x, y, globeHidesPoint );
          if ( !globeHidesPoint ) {
            path << QPointF( x, y );
          }
    }

    GeoDataCoordinates previousModifiedCoords( previousCoords );
    if ( clampToGround ) {
        previousModifiedCoords.setAltitude( 0.0 );
    }
    screenCoordinates( previousModifiedCoords, viewport, x, y, globeHidesPoint );
    if ( !globeHidesPoint ) {
        path << QPointF( x, y );
    }

    qreal  lon = 0.0;
    qreal  lat = 0.0;
    Quaternion  itpos;

    qreal altDiff = currentCoords.altitude() - previousAltitude;

    int startNode = 1;
    const int endNode = tessellatedNodes - 2;

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

        // No "else" here, as this would not add the current point that is required.
        if ( !globeHidesPoint ) {
            path << QPointF( x, y );
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

QRegion AbstractProjection::mapRegion( const ViewportParams *viewport ) const
{
    return QRegion( mapShape( viewport ).toFillPolygon().toPolygon() );
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
