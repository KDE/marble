//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007       Inge Wallin  <ingwa@kde.org>
// Copyright 2007-2012  Torsten Rahn  <rahn@kde.org>
// Copyright 2012       Cezar Mocan <mocancezar@gmail.com>
//

// Local
#include "CylindricalProjection.h"

#include "CylindricalProjection_p.h"

// Marble
#include "GeoDataLinearRing.h"
#include "GeoDataLineString.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "ViewportParams.h"

#include <QPainterPath>

// Maximum amount of nodes that are created automatically between actual nodes.
static const int maxTessellationNodes = 200;

namespace Marble {

CylindricalProjection::CylindricalProjection()
        : AbstractProjection( new CylindricalProjectionPrivate( this ) )
{
}

CylindricalProjection::CylindricalProjection( CylindricalProjectionPrivate* dd )
        : AbstractProjection( dd )
{
}

CylindricalProjection::~CylindricalProjection()
{
}

CylindricalProjectionPrivate::CylindricalProjectionPrivate( CylindricalProjection * parent )
        : AbstractProjectionPrivate( parent ),
          q_ptr( parent )
{

}


QPainterPath CylindricalProjection::mapShape( const ViewportParams *viewport ) const
{
    // Convenience variables
    int  width  = viewport->width();
    int  height = viewport->height();

    qreal  yTop;
    qreal  yBottom;
    qreal  xDummy;

    // Get the top and bottom coordinates of the projected map.
    screenCoordinates( 0.0, maxLat(), viewport, xDummy, yTop );
    screenCoordinates( 0.0, minLat(), viewport, xDummy, yBottom );

    // Don't let the map area be outside the image
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

bool CylindricalProjection::screenCoordinates( const GeoDataLineString &lineString,
                                                  const ViewportParams *viewport,
                                                  QVector<QPolygonF *> &polygons ) const
{

    Q_D( const CylindricalProjection );
    // Compare bounding box size of the line string with the angularResolution
    // Immediately return if the latLonAltBox is smaller.
    if ( !viewport->resolves( lineString.latLonAltBox() ) ) {
    //    mDebug() << "Object too small to be resolved";
        return false;
    }

    QVector<QPolygonF *> subPolygons;
    d->lineStringToPolygon( lineString, viewport, subPolygons );

    polygons << subPolygons;
    return polygons.isEmpty();
}
int CylindricalProjectionPrivate::tessellateLineSegment( const GeoDataCoordinates &aCoords,
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
        int maxTessellationFactor = viewport->radius() < 20000 ? 10 : 20;
        int const finalTessellationPrecision = qBound(2, viewport->radius()/200, maxTessellationFactor) * tessellationPrecision;

        // Let the line segment follow the spherical surface
        // if the distance between the previous point and the current point
        // on screen is too big
        if ( distance > finalTessellationPrecision ) {
            const int tessellatedNodes = qMin<int>( distance / finalTessellationPrecision, maxTessellationNodes );

            mirrorCount = processTessellation( aCoords, bCoords,
                                 tessellatedNodes,
                                 polygons,
                                 viewport,
                                 f,
                                 mirrorCount,
                                 repeatDistance );
        }
        else {
            mirrorCount = crossDateLine( aCoords, bCoords, bx, by, polygons, mirrorCount, repeatDistance );
        }
#ifdef SAFE_DISTANCE
    }
#endif
    return mirrorCount;
}


int CylindricalProjectionPrivate::processTessellation( const GeoDataCoordinates &previousCoords,
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
    for ( int i = 1; i <= tessellatedNodes; ++i ) {
        const qreal t = (qreal)(i) / (qreal)( tessellatedNodes + 1 );

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
        Q_Q(const CylindricalProjection);
        qreal bx, by;
        q->screenCoordinates( currentTessellatedCoords, viewport, bx, by );
        mirrorCount = crossDateLine( previousTessellatedCoords, currentTessellatedCoords, bx, by, polygons,
                                     mirrorCount, repeatDistance );
        previousTessellatedCoords = currentTessellatedCoords;
    }

    // For the clampToGround case add the "current" coordinate after adding all other nodes.
    GeoDataCoordinates currentModifiedCoords( currentCoords );
    if ( clampToGround ) {
        currentModifiedCoords.setAltitude( 0.0 );
    }
    Q_Q(const CylindricalProjection);
    qreal bx, by;
    q->screenCoordinates( currentModifiedCoords, viewport, bx, by );
    mirrorCount = crossDateLine( previousTessellatedCoords, currentModifiedCoords, bx, by, polygons,
                                 mirrorCount, repeatDistance );
    return mirrorCount;
}

int CylindricalProjectionPrivate::crossDateLine( const GeoDataCoordinates & aCoord,
                                                 const GeoDataCoordinates & bCoord,
                                                 qreal bx,
                                                 qreal by,
                                                 QVector<QPolygonF*> &polygons,
                                                 int mirrorCount,
                                                 qreal repeatDistance )
{
    qreal aLon = aCoord.longitude();
    qreal aSign = aLon > 0 ? 1 : -1;

    qreal bLon = bCoord.longitude();
    qreal bSign = bLon > 0 ? 1 : -1;

    qreal delta = 0;
    if( aSign != bSign && fabs(aLon) + fabs(bLon) > M_PI ) {
        int sign = aSign > bSign ? 1 : -1;
        mirrorCount += sign;
    }
    delta = repeatDistance * mirrorCount;
    *polygons.last() << QPointF( bx + delta, by );

    return mirrorCount;
}

bool CylindricalProjectionPrivate::lineStringToPolygon( const GeoDataLineString &lineString,
                                              const ViewportParams *viewport,
                                              QVector<QPolygonF *> &polygons ) const
{
    const TessellationFlags f = lineString.tessellationFlags();

    qreal x = 0;
    qreal y = 0;

    qreal previousX = -1.0;
    qreal previousY = -1.0;

    int mirrorCount = 0;
    qreal distance = repeatDistance( viewport );

    polygons.append( new QPolygonF );

    GeoDataLineString::ConstIterator itCoords = lineString.constBegin();
    GeoDataLineString::ConstIterator itPreviousCoords = lineString.constBegin();

    GeoDataLineString::ConstIterator itBegin = lineString.constBegin();
    GeoDataLineString::ConstIterator itEnd = lineString.constEnd();

    bool processingLastNode = false;

    // We use a while loop to be able to cover linestrings as well as linear rings:
    // Linear rings require to tessellate the path from the last node to the first node
    // which isn't really convenient to achieve with a for loop ...

    const bool isLong = lineString.size() > 10;
    const int maximumDetail = levelForResolution(viewport->angularResolution());
    // The first node of optimized linestrings has a non-zero detail value.
    const bool hasDetail = itBegin->detail() != 0;

    bool isStraight = lineString.latLonAltBox().height() == 0 || lineString.latLonAltBox().width() == 0;

    Q_Q( const CylindricalProjection );
    bool const tesselate = lineString.tessellate();
    bool const isClosed = lineString.isClosed();
    while ( itCoords != itEnd )
    {
        // Optimization for line strings with a big amount of nodes
        bool skipNode = (hasDetail ? itCoords->detail() > maximumDetail
                : isLong && !processingLastNode && itCoords != itBegin &&
                !viewport->resolves( *itPreviousCoords, *itCoords ) );

        if ( !skipNode ) {
            q->screenCoordinates( *itCoords, viewport, x, y );

            // Initializing variables that store the values of the previous iteration
            if ( !processingLastNode && itCoords == itBegin ) {
                itPreviousCoords = itCoords;
                previousX = x;
                previousY = y;
            }

            // This if-clause contains the section that tessellates the line
            // segments of a linestring. If you are about to learn how the code of
            // this class works you can safely ignore this section for a start.
            if ( tesselate && !isStraight) {
                mirrorCount = tessellateLineSegment( *itPreviousCoords, previousX, previousY,
                                           *itCoords, x, y,
                                           polygons, viewport,
                                           f, mirrorCount, distance );
            }

            else {
                // special case for polys which cross dateline but have no Tesselation Flag
                // the expected rendering is a screen coordinates straight line between
                // points, but in projections with repeatX things are not smooth
                mirrorCount = crossDateLine( *itPreviousCoords, *itCoords, x, y, polygons, mirrorCount, distance );
            }

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

        if (isClosed && itCoords == itEnd) {
            itCoords = itBegin;
            processingLastNode = true;
        }
    }

    // Closing e.g. in the Antarctica case.
    // This code makes the assumption that
    // - the first node is located at 180 E
    // - and the last node is located at 180 W
    // TODO: add a similar pattern in the crossDateLine() code.
    /*
    GeoDataLatLonAltBox box = lineString.latLonAltBox();
    if( lineString.isClosed() && box.width() == 2*M_PI ) {
        QPolygonF *poly = polygons.last();
        if( box.containsPole( NorthPole ) ) {
            qreal topMargin = 0.0;
            qreal dummy = 0.0;
            q_ptr->screenCoordinates(0.0, q_ptr->maxLat(), viewport, topMargin, dummy );
            poly->push_back( QPointF( poly->last().x(), topMargin ) );
            poly->push_back( QPointF( poly->first().x(), topMargin ) );
        } else {
            qreal bottomMargin = 0.0;
            qreal dummy = 0.0;
            q_ptr->screenCoordinates(0.0, q_ptr->minLat(), viewport, bottomMargin, dummy );
            poly->push_back( QPointF( poly->last().x(), bottomMargin ) );
            poly->push_back( QPointF( poly->first().x(), bottomMargin ) );
        }
    } */

    repeatPolygons( viewport, polygons );

    return polygons.isEmpty();
}

void CylindricalProjectionPrivate::translatePolygons( const QVector<QPolygonF *> &polygons,
                                                      QVector<QPolygonF *> &translatedPolygons,
                                                      qreal xOffset )
{
    // mDebug() << "Translation: " << xOffset;
    translatedPolygons.reserve(polygons.size());

    QVector<QPolygonF *>::const_iterator itPolygon = polygons.constBegin();
    QVector<QPolygonF *>::const_iterator itEnd = polygons.constEnd();

    for( ; itPolygon != itEnd; ++itPolygon ) {
        QPolygonF * polygon = new QPolygonF;
        *polygon = **itPolygon;
        polygon->translate( xOffset, 0 );
        translatedPolygons.append( polygon );
    }
}

void CylindricalProjectionPrivate::repeatPolygons( const ViewportParams *viewport,
                                                QVector<QPolygonF *> &polygons ) const
{
    Q_Q( const CylindricalProjection );

    qreal xEast = 0;
    qreal xWest = 0;
    qreal y = 0;

    // Choose a latitude that is inside the viewport.
    qreal centerLatitude = viewport->viewLatLonAltBox().center().latitude();
    
    GeoDataCoordinates westCoords( -M_PI, centerLatitude );
    GeoDataCoordinates eastCoords( +M_PI, centerLatitude );

    q->screenCoordinates( westCoords, viewport, xWest, y );
    q->screenCoordinates( eastCoords, viewport, xEast, y );

    if ( xWest <= 0 && xEast >= viewport->width() - 1 ) {
        // mDebug() << "No repeats";
        return;
    }

    qreal repeatXInterval = xEast - xWest;

    qreal repeatsLeft  = 0;
    qreal repeatsRight = 0;

    if ( xWest > 0 ) {
        repeatsLeft = (int)( xWest / repeatXInterval ) + 1;
    }
    if ( xEast < viewport->width() ) {
        repeatsRight = (int)( ( viewport->width() - xEast ) / repeatXInterval ) + 1;
    }

    QVector<QPolygonF *> repeatedPolygons;
    QVector<QPolygonF *> translatedPolygons;

    qreal xOffset = 0;
    qreal it = repeatsLeft;
    
    while ( it > 0 ) {
        xOffset = -it * repeatXInterval;
        translatePolygons( polygons, translatedPolygons, xOffset );
        repeatedPolygons << translatedPolygons;
        translatedPolygons.clear();
        --it;
    }

    repeatedPolygons << polygons;

    it = 1;

    while ( it <= repeatsRight ) {
        xOffset = +it * repeatXInterval;
        translatePolygons( polygons, translatedPolygons, xOffset );
        repeatedPolygons << translatedPolygons;
        translatedPolygons.clear();
        ++it;
    }

    polygons = repeatedPolygons;

    // mDebug() << Q_FUNC_INFO << "Coordinates: " << xWest << xEast
    //          << "Repeats: " << repeatsLeft << repeatsRight;
}

qreal CylindricalProjectionPrivate::repeatDistance( const ViewportParams *viewport ) const
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

}

