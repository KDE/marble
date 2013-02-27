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

AbstractProjection::AbstractProjection( AbstractProjectionPrivate* dd )
    : d_ptr( dd )
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

qreal AbstractProjectionPrivate::repeatDistance( const ViewportParams *viewport ) const
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


int AbstractProjectionPrivate::tessellateLineSegment( const GeoDataCoordinates &aCoords,
                                                qreal ax, qreal ay,
                                                const GeoDataCoordinates &bCoords,
                                                qreal bx, qreal by,
                                                QVector<QPolygonF*> &polygons,
                                                const ViewportParams *viewport,
                                                TessellationFlags f,
                                                int mirrorCount,
                                                qreal repeatDistance) const
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

            mirrorCount = processTessellation( aCoords, bCoords,
                                 tessellatedNodes,
                                 polygons,
                                 viewport,
                                 f,
                                 mirrorCount,
                                 repeatDistance );
        }
        else {
            mirrorCount = crossDateLine( aCoords, bCoords, polygons, viewport, mirrorCount, repeatDistance );
        }
#ifdef SAFE_DISTANCE
    }
#endif
    return mirrorCount;
}


int AbstractProjectionPrivate::processTessellation( const GeoDataCoordinates &previousCoords,
                                                    const GeoDataCoordinates &currentCoords,
                                                    int tessellatedNodes,
                                                    QVector<QPolygonF*> &polygons,
                                                    const ViewportParams *viewport,
                                                    TessellationFlags f,
                                                    int mirrorCount,
                                                    qreal repeatDistance) const
{

    const bool clampToGround = f.testFlag( FollowGround );
    const bool followLatitudeCircle = f.testFlag( RespectLatitudeCircle )
                                      && previousCoords.latitude() == currentCoords.latitude();

    // Maximum amount of tessellation nodes.
    if ( tessellatedNodes > maxTessellationNodes ) tessellatedNodes = maxTessellationNodes;

    // Calculate steps for tessellation: lonDiff and altDiff
    qreal lonDiff = 0.0;
    if ( followLatitudeCircle ) {
        const int previousSign = previousCoords.longitude() > 0 ? 1 : -1;
        const int currentSign = currentCoords.longitude() > 0 ? 1 : -1;

        lonDiff = currentCoords.longitude() - previousCoords.longitude();
        if ( previousSign != currentSign
             && fabs(previousCoords.longitude()) + fabs(currentCoords.longitude()) > M_PI ) {
            if ( previousSign > currentSign ) {
                // going eastwards ->
                lonDiff += 2 * M_PI ;
            } else {
                // going westwards ->
                lonDiff -= 2 * M_PI;
            }
        }
        if ( fabs( lonDiff ) == 2 * M_PI ) {
            return mirrorCount;
        }
    }

    const qreal altDiff = currentCoords.altitude() - previousCoords.altitude();

    // Create the tessellation nodes.
    GeoDataCoordinates previousTessellatedCoords = previousCoords;
    for ( int i = 1; i <= tessellatedNodes - 2; ++i ) {
        const qreal t = (qreal)(i) / (qreal)( tessellatedNodes );

        // interpolate the altitude, too
        const qreal altitude = clampToGround ? 0 : altDiff * t + previousCoords.altitude();

        qreal lon = 0.0;
        qreal lat = 0.0;
        if ( followLatitudeCircle ) {
            // To tessellate along latitude circles use the 
            // linear interpolation of the longitude.
            lon = lonDiff * t + previousCoords.longitude();
            lat = previousTessellatedCoords.latitude();
        }
        else {
            // To tessellate along great circles use the 
            // normalized linear interpolation ("NLERP") for latitude and longitude.
            const Quaternion itpos = Quaternion::nlerp( previousCoords.quaternion(), currentCoords.quaternion(), t );
            itpos. getSpherical( lon, lat );
        }

        const GeoDataCoordinates currentTessellatedCoords( lon, lat, altitude );
        mirrorCount = crossDateLine( previousTessellatedCoords, currentTessellatedCoords, polygons, viewport,
                                     mirrorCount, repeatDistance );
        previousTessellatedCoords = currentTessellatedCoords;
    }

    // For the clampToGround case add the "current" coordinate after adding all other nodes. 
    GeoDataCoordinates currentModifiedCoords( currentCoords );
    if ( clampToGround ) {
        currentModifiedCoords.setAltitude( 0.0 );
    }
    mirrorCount = crossDateLine( previousTessellatedCoords, currentModifiedCoords, polygons, viewport,
                                 mirrorCount, repeatDistance );
    return mirrorCount;
}

int AbstractProjectionPrivate::crossDateLine( const GeoDataCoordinates & aCoord,
                                              const GeoDataCoordinates & bCoord,
                                              QVector<QPolygonF*> &polygons,
                                              const ViewportParams *viewport,
                                              int mirrorCount,
                                              qreal repeatDistance ) const
{
    qreal aLon = aCoord.longitude();
    qreal aSign = aLon > 0 ? 1 : -1;

    qreal bLon = bCoord.longitude();
    qreal bSign = bLon > 0 ? 1 : -1;

    qreal x, y;
    bool globeHidesPoint;

    Q_Q( const AbstractProjection );

    q->screenCoordinates( bCoord, viewport, x, y, globeHidesPoint );

    int sign = 0;
    if( !globeHidesPoint ) {

        qreal delta = 0;
        if( aSign != bSign
                && fabs(aLon) + fabs(bLon) > M_PI
                && q->repeatX() ) {
            sign = aSign > bSign ? 1 : -1;
            mirrorCount += sign;
        }
        delta = repeatDistance * mirrorCount;
        *polygons.last() << QPointF( x +  delta, y );
    }
    else {
        if ( !polygons.last()->isEmpty() ) {
            QPolygonF *path = new QPolygonF;
            polygons.append( path );
        }
    }

    return mirrorCount;
}
