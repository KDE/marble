//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007        Inge Wallin   <ingwa@kde.org>
// Copyright 2007-2012   Torsten Rahn  <rahn@kde.org>
// Copyright 2012        Cezar Mocan   <mocancezar@gmail.com>
//

// Local
#include "AbstractProjection.h"

#include "AbstractProjection_p.h"

#include "MarbleDebug.h"
#include <QtGui/QRegion>

// Marble
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "ViewportParams.h"

using namespace Marble;

// Maximum amount of nodes that are created automatically between actual nodes.
static const int maxTessellationNodes = 200;

AbstractProjection::AbstractProjection()
    : d_ptr( new AbstractProjectionPrivate( this ) )
{
}

AbstractProjection::AbstractProjection( AbstractProjectionPrivate &dd )
    : d_ptr( &dd )
{
}

AbstractProjection::~AbstractProjection()
{
}

AbstractProjectionPrivate::AbstractProjectionPrivate( AbstractProjection * parent )
    : m_repeatX(false),
      m_maxLat(0),
      m_minLat(0),
      q_ptr( parent)
{
}

qreal AbstractProjection::maxValidLat() const
{
    return +90.0 * DEG2RAD;
}

qreal AbstractProjection::maxLat() const
{
    Q_D(const AbstractProjection );
    return d->m_maxLat;
}

void AbstractProjection::setMaxLat( qreal maxLat )
{
    if ( maxLat < maxValidLat() ) {
        mDebug() << Q_FUNC_INFO << "Trying to set maxLat to a value that is out of the valid range.";
        return;
    }

    Q_D( AbstractProjection );
    d->m_maxLat = maxLat;
}

qreal AbstractProjection::minValidLat() const
{
    return -90.0 * DEG2RAD;
}

qreal AbstractProjection::minLat() const
{
    Q_D( const AbstractProjection );
    return d->m_minLat;
}

void AbstractProjection::setMinLat( qreal minLat )
{
    if ( minLat < minValidLat() ) {
        mDebug() << Q_FUNC_INFO << "Trying to set minLat to a value that is out of the valid range.";
        return;
    }

    Q_D( AbstractProjection );
    d->m_minLat = minLat;
}

bool AbstractProjection::repeatableX() const
{
    return true;
}

bool AbstractProjection::repeatX() const
{
    Q_D( const AbstractProjection );
    return d->m_repeatX;
}

void AbstractProjection::setRepeatX( bool repeatX )
{
    if ( repeatX && !repeatableX() ) {
        mDebug() << Q_FUNC_INFO << "Trying to repeat a projection that is not repeatable";
        return;
    }

    Q_D( AbstractProjection );
    d->m_repeatX = repeatX;
}

bool AbstractProjection::screenCoordinates( const GeoDataCoordinates &geopoint, 
                                            const ViewportParams *viewport,
                                            qreal &x, qreal &y ) const
{
    bool globeHidesPoint;

    return screenCoordinates( geopoint, viewport, x, y, globeHidesPoint );
}

bool AbstractProjection::screenCoordinates( const GeoDataCoordinates &geopoint,
                                            const ViewportParams *viewport,
                                            QPointF &screenpoint ) const
{
    bool visible;
    qreal x(0), y(0);
    visible = screenCoordinates( geopoint, viewport, x, y );
    screenpoint = QPointF( x,y );
    return visible;
}

bool AbstractProjection::screenCoordinates( const GeoDataCoordinates &coordinates,
                                    const ViewportParams *viewport,
                                    qreal *x, qreal &y, int &pointRepeatNum, bool &globeHidesPoint ) const
{
    return screenCoordinates( coordinates, viewport, x, y, pointRepeatNum, 
           QSizeF( 0.0, 0.0 ), globeHidesPoint );
}

qreal AbstractProjectionPrivate::mirrorPoint( const ViewportParams *viewport ) const
{
    // Choose a latitude that is inside the viewport.
    qreal centerLatitude = viewport->viewLatLonAltBox().center().latitude();

    GeoDataCoordinates westCoords( -M_PI, centerLatitude );
    GeoDataCoordinates eastCoords( +M_PI, centerLatitude );
    qreal xWest, xEast, dummyY;

    Q_Q( const AbstractProjection );

    q->screenCoordinates( westCoords, viewport, xWest, dummyY );
    q->screenCoordinates( eastCoords, viewport, xEast, dummyY );

    return   xEast - xWest;
}


void AbstractProjectionPrivate::translatePolygons( const QVector<QPolygonF *> &polygons,
                                                   QVector<QPolygonF *> &translatedPolygons,
                                                   qreal xOffset ) const
{
//    mDebug() << "Translation: " << xOffset;

    QVector<QPolygonF *>::const_iterator itPolygon = polygons.constBegin();
    QVector<QPolygonF *>::const_iterator itEnd = polygons.constEnd();
    
    for( ; itPolygon != itEnd; ++itPolygon ) {
        QPolygonF * polygon = new QPolygonF;
        *polygon = **itPolygon;
        polygon->translate( xOffset, 0 );
        translatedPolygons.append( polygon );
    }
}



GeoDataLatLonAltBox AbstractProjection::latLonAltBox( const QRect& screenRect,
                                                      const ViewportParams *viewport ) const
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

    // We need a point on the screen at maxLat that definitely gets displayed:

    // FIXME: Some of the following code can be safely removed as soon as we properly handle
    //        GeoDataLinearRing::latLonAltBox().
    qreal averageLongitude = ( latLonAltBox.west() + latLonAltBox.east() ) / 2.0;

    GeoDataCoordinates maxLatPoint( averageLongitude, maxLat(), 0.0, GeoDataCoordinates::Radian );
    GeoDataCoordinates minLatPoint( averageLongitude, minLat(), 0.0, GeoDataCoordinates::Radian );

    qreal dummyX, dummyY; // not needed

    if ( latLonAltBox.north() > maxLat() ||
         screenCoordinates( maxLatPoint, viewport, dummyX, dummyY ) ) {
        latLonAltBox.setNorth( maxLat() );
    }
    if ( latLonAltBox.north() < minLat() ||
         screenCoordinates( minLatPoint, viewport, dummyX, dummyY ) ) {
        latLonAltBox.setSouth( minLat() );
    }

    latLonAltBox.setMinAltitude(      -100000000.0 );
    latLonAltBox.setMaxAltitude( 100000000000000.0 );

    return latLonAltBox;
}


QRegion AbstractProjection::mapRegion( const ViewportParams *viewport ) const
{
    return QRegion( mapShape( viewport ).toFillPolygon().toPolygon() );
}


void AbstractProjectionPrivate::tessellateLineSegment( const GeoDataCoordinates &aCoords,
                                                qreal ax, qreal ay,
                                                const GeoDataCoordinates &bCoords,
                                                qreal bx, qreal by,
                                                QVector<QPolygonF*> &polygons,
                                                const ViewportParams *viewport,
                                                const GeoDataLineString *lineString,
                                                TessellationFlags f ) const
{
    // We take the manhattan length as a distance approximation
    // that can be too big by a factor of sqrt(2)
    qreal distance = fabs((bx - ax)) + fabs((by - ay));
#ifdef SAFE_DISTANCE
    // Interpolate additional nodes if the line segment that connects the
    // current or previous nodes might cross the viewport.
    // The latter can pretty safely be excluded for most projections if both points
    // are located on the same side relative to the viewport boundaries and if they are
    // located more than half the line segment distance away from the viewport.
    const qreal safeDistance = - 0.5 * distance;
    if (   !( bx < safeDistance && ax < safeDistance )
        || !( by < safeDistance && ay < safeDistance )
        || !( bx + safeDistance > viewport->width()
            && ax + safeDistance > viewport->width() )
        || !( by + safeDistance > viewport->height()
            && ay + safeDistance > viewport->height() )
    )
    {
#endif
        bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
        int const finalTessellationPrecision = smallScreen ? 3 * tessellationPrecision : tessellationPrecision;

        int tessellatedNodes = (int)( distance / finalTessellationPrecision );

        // Let the line segment follow the spherical surface
        // if the distance between the previous point and the current point
        // on screen is too big
        if ( distance > finalTessellationPrecision ) {

            processTessellation( aCoords, bCoords,
                                 tessellatedNodes,
                                 polygons,
                                 viewport,
                                 lineString,
                                 f );
        }
        else {
            crossDateLine( aCoords, bCoords, polygons, viewport, lineString );
        }
#ifdef SAFE_DISTANCE
    }
#endif
}


void AbstractProjectionPrivate::processTessellation(  const GeoDataCoordinates &previousCoords,
                                                    const GeoDataCoordinates &currentCoords,
                                                    int tessellatedNodes,
                                                    QVector<QPolygonF*> &polygons,
                                                    const ViewportParams *viewport,
                                                    const GeoDataLineString *lineString,
                                                    TessellationFlags f ) const
{

    const bool clampToGround = f.testFlag( FollowGround );
    bool followLatitudeCircle = false;     

    // Maximum amount of tessellation nodes.
    if ( tessellatedNodes > maxTessellationNodes ) tessellatedNodes = maxTessellationNodes;

    qreal previousAltitude = previousCoords.altitude();

    // Calculate steps for tessellation: lonDiff and altDiff 
    qreal lonDiff = 0.0;
    qreal previousLongitude = 0.0;
    qreal previousLatitude = 0.0;
    previousCoords.geoCoordinates( previousLongitude, previousLatitude );
    qreal previousSign = previousLongitude > 0 ? 1 : -1;

    qreal currentLongitude = 0.0;
    qreal currentLatitude = 0.0;
    currentCoords.geoCoordinates( currentLongitude, currentLatitude );
    qreal currentSign = currentLongitude > 0 ? 1 : -1;

    if ( f.testFlag( RespectLatitudeCircle )
         && previousLatitude == currentLatitude ) {
        followLatitudeCircle = true;
        lonDiff = currentLongitude - previousLongitude;
        if ( previousSign != currentSign
             && fabs(previousLongitude) + fabs(currentLongitude) > M_PI ) {
            if ( previousSign > currentSign ) {
                // going eastwards ->
                lonDiff += 2 * M_PI ;
            } else {
                // going westwards ->
                lonDiff -= 2 * M_PI;
            }
        }
        if ( fabs( lonDiff ) == 2 * M_PI ) {
            return;
        }
    }

    qreal  lon = 0.0;
    qreal  lat = 0.0;

    qreal altDiff = currentCoords.altitude() - previousAltitude;

    int startNode = 1;
    const int endNode = tessellatedNodes - 2;

    // Create the tessellation nodes.
    for ( int i = startNode; i <= endNode; ++i ) {
        qreal  t = (qreal)(i) / (qreal)( tessellatedNodes ) ;

        // interpolate the altitude, too
        qreal altitude = clampToGround ? 0 : altDiff * t + previousAltitude;

        if ( followLatitudeCircle ) {
            // To tessellate along latitude circles use the 
            // linear interpolation of the longitude.
            lon = lonDiff * t + previousCoords.longitude();
            lat = previousLatitude;
        }
        else {
            // To tessellate along great circles use the 
            // normalized linear interpolation ("NLERP") for latitude and longitude.
            const Quaternion itpos = Quaternion::nlerp( previousCoords.quaternion(), currentCoords.quaternion(), t );
            itpos. getSpherical( lon, lat );
        }

        crossDateLine( GeoDataCoordinates( previousLongitude, previousLatitude, previousAltitude),
                       GeoDataCoordinates( lon, lat, altitude ), polygons, viewport, lineString );
        previousLongitude = lon;
    }


    // For the clampToGround case add the "current" coordinate after adding all other nodes. 
    GeoDataCoordinates currentModifiedCoords( currentCoords );
    if ( clampToGround ) {
        currentModifiedCoords.setAltitude( 0.0 );
    }
    crossDateLine( GeoDataCoordinates( previousLongitude, previousLatitude, previousAltitude ),
                   currentModifiedCoords, polygons, viewport, lineString );
}

void AbstractProjectionPrivate::crossDateLine( const GeoDataCoordinates & aCoord,
                                               const GeoDataCoordinates & bCoord,
                                               QVector<QPolygonF*> &polygons,
                                               const ViewportParams *viewport,
                                               const GeoDataLineString *lineString ) const
{
    qreal aLon = aCoord.longitude();
    qreal bLon = bCoord.longitude();

    qreal aLat = aCoord.latitude();
    qreal bLat = bCoord.latitude();

    GeoDataCoordinates::normalizeLonLat( aLon, aLat );
    GeoDataCoordinates::normalizeLonLat( bLon, bLat );

    qreal aSign = aLon > 0 ? 1 : -1;
    qreal bSign = bLon > 0 ? 1 : -1;

//    fprintf( stderr, "Enter crossDateLine (%.3lf %.3lf) (%.3lf %.3lf)\n", aLon, aLat, bLon, bLat );

    qreal x, y;
    bool globeHidesPoint;

    Q_Q( const AbstractProjection );

    q->screenCoordinates( bCoord, viewport, x, y, globeHidesPoint );

    if( !globeHidesPoint ) {

        if( aSign != bSign
                && fabs(aLon) + fabs(bLon) > M_PI
                && q->repeatX() ) {
            qreal delta = mirrorPoint( viewport );
            if ( aSign > bSign ) {
                // going eastwards ->
                *polygons.last() << QPointF( x +  delta, y );
            } else {
                // going westwards <-
                *polygons.last() << QPointF( x -  delta, y );
            }
            QPolygonF *path = new QPolygonF;
            polygons.append( path );


//            fprintf( stderr, "Before: (%lf %lf) (%lf %lf)\n", aLon, aLat, bLon, bLat );

            if ( aLat < 0 && bLat < 0 && lineString->latLonAltBox().containsPole( AnyPole ) ) {

                qreal southernIntersectionFirst = lineString->southernMostIDLCrossing().first.latitude();
                qreal southernIntersectionSecond = lineString->southernMostIDLCrossing().second.latitude();

                if ( southernIntersectionFirst > southernIntersectionSecond ) {
                    southernIntersectionFirst = lineString->southernMostIDLCrossing().second.latitude();
                    southernIntersectionSecond = lineString->southernMostIDLCrossing().first.latitude();
                }
 

                if ( southernIntersectionFirst <= aLat && aLat <= southernIntersectionSecond &&
                        southernIntersectionFirst <= bLat && bLat <= southernIntersectionSecond ) {
                    int sgnCrossing = ( lineString->southernMostIDLCrossing().first.longitude() > 0 ) ? 1 : -1; // 1 for east->west, -1 for west->east


                    fprintf( stderr, "crossDateLine checked :) %d  (%lf %lf), (%lf %lf) ->  (%lf %lf)\n", lineString->size(), aLon, aLat, bLon, bLat, southernIntersectionFirst, southernIntersectionSecond );

                    fprintf( stderr, "minLat = %lf\n", m_minLat );
                    GeoDataCoordinates southPolePositive( +M_PI, m_minLat );
                    GeoDataCoordinates southPoleNegative( -M_PI, m_minLat );

                    qreal positiveX, positiveY, negativeX, negativeY;
                           
                    q->screenCoordinates( southPolePositive, viewport, positiveX, positiveY, globeHidesPoint );
                    q->screenCoordinates( southPoleNegative, viewport, negativeX, negativeY, globeHidesPoint );                   


                    if ( sgnCrossing == -1 ) 
                        *polygons.last() << QPointF( positiveX, positiveY ) << QPointF( negativeX, negativeY );
                    else
                        *polygons.last() << QPointF( negativeX, negativeY ) << QPointF( positiveX, positiveY );
                }

            }

            if ( aLat > 0 && bLat > 0 && lineString->latLonAltBox().containsPole( AnyPole )) {

            }
        }

        *polygons.last() << QPointF( x, y );
    }
}
