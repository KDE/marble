//
// This file is part of the Marble Virtual Globe.
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
    : d( new AbstractProjectionPrivate( this ) )
{
}

AbstractProjection::~AbstractProjection()
{
    delete d;
}

AbstractProjectionPrivate::AbstractProjectionPrivate( AbstractProjection * _q )
    : q( _q )
{
    m_repeatX = q->repeatableX();
    m_maxLat = q->maxValidLat();
    m_minLat = q->minValidLat();
}

qreal AbstractProjection::maxValidLat() const
{
    return +90.0 * DEG2RAD;
}

qreal AbstractProjection::maxLat() const
{
    return d->m_maxLat;
}

void AbstractProjection::setMaxLat( qreal maxLat )
{
    if ( maxLat < maxValidLat() ) {
        mDebug() << Q_FUNC_INFO << "Trying to set maxLat to a value that is out of the valid range.";
        return;
    }
    d->m_maxLat = maxLat;
}

qreal AbstractProjection::minValidLat() const
{
    return -90.0 * DEG2RAD;
}

qreal AbstractProjection::minLat() const
{
    return d->m_minLat;
}

void AbstractProjection::setMinLat( qreal minLat )
{
    if ( minLat < minValidLat() ) {
        mDebug() << Q_FUNC_INFO << "Trying to set minLat to a value that is out of the valid range.";
        return;
    }
    d->m_minLat = minLat;
}

bool AbstractProjection::repeatableX() const
{
    return true;
}

bool AbstractProjection::repeatX() const
{
    return d->m_repeatX;
}

void AbstractProjection::setRepeatX( bool repeatX )
{
    if ( repeatX && !repeatableX() ) {
        mDebug() << Q_FUNC_INFO << "Trying to repeat a projection that is not repeatable";
        return;
    }

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

bool AbstractProjection::screenCoordinates( const GeoDataLineString &lineString,
                                                  const ViewportParams *viewport,
                                                  QVector<QPolygonF *> &polygons ) const
{
    // Compare bounding box size of the line string with the angularResolution
    // Immediately return if the latLonAltBox is smaller.
    if ( !viewport->resolves( lineString.latLonAltBox() ) ) {
//      mDebug() << "Object too small to be resolved";
        return false;
    }

    QVector<GeoDataLineString*> lineStrings;

    if (
         ( !traversablePoles() && lineString.latLonAltBox().containsPole( AnyPole ) ) ||
         ( lineString.latLonAltBox().crossesDateLine() )
       ) {
        // We correct for Poles and DateLines:
        lineStrings = lineString.toRangeCorrected();

        foreach ( GeoDataLineString * itLineString, lineStrings ) {
            QVector<QPolygonF *> subPolygons;

            lineStringToPolygon( *itLineString, viewport, subPolygons );
            polygons << subPolygons;
        }
    }
    else {
        lineStringToPolygon( lineString, viewport, polygons );
    }

    return polygons.isEmpty();
}

bool AbstractProjection::lineStringToPolygon( const GeoDataLineString &lineString,
                                              const ViewportParams *viewport,
                                              QVector<QPolygonF *> &polygons ) const
{
    const TessellationFlags f = lineString.tessellationFlags();

    qreal x = 0;
    qreal y = 0;
    bool globeHidesPoint = false;

    qreal previousX = -1.0;
    qreal previousY = -1.0;
    bool previousGlobeHidesPoint = false;

    qreal horizonX = -1.0;
    qreal horizonY = -1.0;
    bool isAtHorizon = false;

    QPolygonF * polygon = new QPolygonF;

    GeoDataLineString::ConstIterator itCoords = lineString.constBegin();
    GeoDataLineString::ConstIterator itPreviousCoords = lineString.constBegin();

    // Some projections display the earth in a way so that there is a
    // foreside and a backside.
    // The horizon is the line (usually a circle) which separates both
    // sides from each other and resembles the map shape.
    GeoDataCoordinates horizonCoords;

    // A horizon pair is a pair of two subsequent horizon crossings:
    // The first one describes the point where a line string disappears behind the horizon.
    // and where horizonPair is set to true.
    // The second one describes the point where the line string reappears.
    // In this case the two points are connected and horizonPair is set to false again.
    bool horizonPair = false;
    GeoDataCoordinates horizonDisappearCoords;

    // If the first horizon crossing in a line string describes the appearance of
    // a line string then we call it a "horizon orphan" and horizonOrphan is set to true.
    // In this case once the last horizon crossing in the line string is reached
    // it needs to be connected to the orphan.
    bool horizonOrphan = false;
    GeoDataCoordinates horizonOrphanCoords;


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
        isAtHorizon = false;

        // Optimization for line strings with a big amount of nodes
        bool skipNode = itCoords != itBegin && isLong && !processingLastNode &&
                        viewport->resolves( *itPreviousCoords, *itCoords );

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
            isAtHorizon = ( globeHidesPoint || previousGlobeHidesPoint ) &&
                          ( globeHidesPoint !=  previousGlobeHidesPoint );
     
            if ( isAtHorizon ) {
                // Handle the "horizon case"
                horizonCoords = d->findHorizon( previousCoords, currentCoords, viewport, f );

                if ( lineString.isClosed() ) {
                    if ( horizonPair ) {
                        horizonToPolygon( viewport, horizonDisappearCoords, horizonCoords, polygon );
                        horizonPair = false;
                    }
                    else {
                        d->manageHorizonCrossing( globeHidesPoint, horizonCoords,
                                                  horizonPair, horizonDisappearCoords,
                                                  horizonOrphan, horizonOrphanCoords );
                    }
                }

                screenCoordinates( horizonCoords, viewport, horizonX, horizonY );

                // If the line appears on the visible half we need
                // to add an interpolated point at the horizon as the previous point.
                if ( previousGlobeHidesPoint ) {
                    polygon->append( QPointF( horizonX, horizonY ) );
                }
            }

            // This if-clause contains the section that tessellates the line
            // segments of a linestring. If you are about to learn how the code of
            // this class works you can safely ignore this section for a start.

            if ( lineString.tessellate() /* && ( isVisible || previousIsVisible ) */ ) {

                if ( !isAtHorizon ) {

                    tessellateLineSegment( previousCoords, previousX, previousY,
                                           currentCoords, x, y,
                                           polygon, viewport,
                                           f );

                }
                else {
                    // Connect the interpolated  point at the horizon with the
                    // current or previous point in the line. 
                    if ( previousGlobeHidesPoint ) {
                        tessellateLineSegment( horizonCoords, horizonX, horizonY,
                                               currentCoords, x, y,
                                               polygon, viewport,
                                               f );
                    }
                    else {
                        tessellateLineSegment( previousCoords, previousX, previousY,
                                               horizonCoords, horizonX, horizonY,
                                               polygon, viewport,
                                               f );
                    }
                }
            }
            else {
                if ( !globeHidesPoint ) {
                    polygon->append( QPointF( x, y ) );
                }
                else {
                    if ( !previousGlobeHidesPoint && isAtHorizon ) {
                        polygon->append( QPointF( horizonX, horizonY ) );
                    }
                }
            }

            if ( globeHidesPoint ) {
                if (   !previousGlobeHidesPoint
                    && !lineString.isClosed()
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

    // In case of horizon crossings, make sure that we always get a
    // polygon closed correctly.
    if ( horizonOrphan && lineString.isClosed() ) {
        horizonToPolygon( viewport, horizonCoords, horizonOrphanCoords, polygon );
    }

    if ( polygon->size() > 1 ){
        polygons.append( polygon );
    }
    else {
        delete polygon; // Clean up "unused" empty polygon instances
    }

    d->repeatPolygons( viewport, polygons );

    return polygons.isEmpty();
}


void AbstractProjectionPrivate::repeatPolygons( const ViewportParams *viewport,
                                                QVector<QPolygonF *> &polygons ) const
{
    if ( !q->repeatX() ) {
        // The projection doesn't allow repeats in direction of the x-axis
        return;
    }
    
    bool globeHidesPoint = false;

    qreal xEast = 0;
    qreal xWest = 0;
    qreal y = 0;

    // Choose a latitude that is inside the viewport.
    qreal centerLatitude = viewport->viewLatLonAltBox().center().latitude();
    
    GeoDataCoordinates westCoords( -M_PI, centerLatitude );
    GeoDataCoordinates eastCoords( +M_PI, centerLatitude );

    q->screenCoordinates( westCoords, viewport, xWest, y, globeHidesPoint );
    q->screenCoordinates( eastCoords, viewport, xEast, y, globeHidesPoint );

    if ( xWest <= 0 && xEast >= viewport->width() - 1 ) {
//        mDebug() << "No repeats";
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

//    mDebug() << Q_FUNC_INFO << "Coordinates: " << xWest << xEast
//             << "Repeats: " << repeatsLeft << repeatsRight;
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


void AbstractProjectionPrivate::manageHorizonCrossing( bool globeHidesPoint,
                                                const GeoDataCoordinates& horizonCoords,
                                                bool& horizonPair,
                                                GeoDataCoordinates& horizonDisappearCoords,
                                                bool& horizonOrphan,
                                                GeoDataCoordinates& horizonOrphanCoords ) const
{
    if ( !horizonPair ) {
        if ( globeHidesPoint ) {
            horizonDisappearCoords = horizonCoords;
            horizonPair = true;
        }
        else {
            horizonOrphanCoords = horizonCoords;
            horizonOrphan = true;
        }
    }
}

void AbstractProjection::horizonToPolygon( const ViewportParams *viewport,
                                           const GeoDataCoordinates & disappearCoords,
                                           const GeoDataCoordinates & reappearCoords,
                                           QPolygonF * polygon ) const
{
    qreal x, y;

    const qreal imageHalfWidth  = viewport->width() / 2;
    const qreal imageHalfHeight = viewport->height() / 2;

    // Calculate the angle of the position vectors of both coordinates
    screenCoordinates( disappearCoords, viewport, x, y );
    qreal alpha = atan2( y - imageHalfHeight,
                         x - imageHalfWidth );

    screenCoordinates( reappearCoords, viewport, x, y );
    qreal beta =  atan2( y - imageHalfHeight,
                         x - imageHalfWidth );

    // Calculate the difference between both
    qreal diff = GeoDataCoordinates::normalizeLon( beta - alpha );

    qreal sgndiff = diff < 0 ? -1 : 1;

    qreal itx, ity;
    const qreal arcradius = viewport->radius();
    const int itEnd = fabs(diff * RAD2DEG);

    // Create a polygon that resembles an arc between the two position vectors
    for ( int it = 0; it <= itEnd; ++it ) {
        qreal angle = alpha + DEG2RAD * sgndiff * it;
        itx = imageHalfWidth  +  arcradius * cos( angle );
        ity = imageHalfHeight +  arcradius * sin( angle );
        *polygon << QPointF( itx, ity );
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


bool AbstractProjection::exceedsLatitudeRange( const GeoDataLineString &lineString ) const
{
    GeoDataLatLonAltBox latLonAltBox = lineString.latLonAltBox();

    return (    latLonAltBox.north() >= maxLat()
             || latLonAltBox.south() <= minLat() );
}


bool AbstractProjection::exceedsLatitudeRange( const GeoDataCoordinates &coords ) const
{
    qreal lat = coords.latitude();
    // Evaluate the most likely case first:
    // The case where we are within the range and where our latitude is normalized
    // to the range of 90 deg S ... 90 deg N
    if ( lat < maxLat() && lat > minLat() ) {
        return false;
    }
    else {
        // If we are not within the range then normalize the latitude and check again.
        qreal normalizedLat = GeoDataCoordinates::normalizeLat( lat );
        if ( normalizedLat == lat ) {
            return true;
        }

        if ( normalizedLat < maxLat() && normalizedLat > minLat() ) {
            // FIXME: Should we just normalize latitude and longitude and be done?
            //        While this might work well for persistent data it would create some 
            //        possible overhead for temporary data, so this needs careful thinking.
            mDebug() << "GeoDataCoordinates not normalized!";
            return false;
        }
    }

    // We have exceeded the range.
    return true;
}


QRegion AbstractProjection::mapRegion( const ViewportParams *viewport ) const
{
    return QRegion( mapShape( viewport ).toFillPolygon().toPolygon() );
}


void AbstractProjection::tessellateLineSegment( const GeoDataCoordinates &aCoords,
                                                qreal ax, qreal ay,
                                                const GeoDataCoordinates &bCoords,
                                                qreal bx, qreal by,
                                                QPolygonF * polygon,
                                                const ViewportParams *viewport,
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

            *polygon << d->processTessellation( aCoords, bCoords,
                                        tessellatedNodes, viewport,
                                        f );
        }
        else {
            QPolygonF path;
            qreal x = 0.0;
            qreal y = 0.0;
            bool globeHidesPoint = false;

            screenCoordinates( aCoords, viewport, x, y, globeHidesPoint );
            if ( !globeHidesPoint ) {
                path << QPointF( x, y );
            }
            screenCoordinates( bCoords, viewport, x, y, globeHidesPoint );
            if ( !globeHidesPoint ) {
                path << QPointF( x, y );
            }
            *polygon << path;
        }
#ifdef SAFE_DISTANCE
    }
#endif
}


QPolygonF AbstractProjectionPrivate::processTessellation(  const GeoDataCoordinates &previousCoords,
                                                    const GeoDataCoordinates &currentCoords,
                                                    int tessellatedNodes,
                                                    const ViewportParams *viewport,
                                                    TessellationFlags f ) const
{
    QPolygonF   path;

    const bool clampToGround = f.testFlag( FollowGround );
    bool followLatitudeCircle = false;     

    // Maximum amount of tessellation nodes.
    if ( tessellatedNodes > maxTessellationNodes ) tessellatedNodes = maxTessellationNodes;

//    mDebug() << "Creating tessellation nodes:" << tessellatedNodes;

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
            // FIXME: Take dateline into account
            lonDiff = currentLongitude - previousLongitude;
            if ( fabs( lonDiff ) == 2 * M_PI ) {
                return path;
            }
        }
        else {
//            mDebug() << "Don't FollowLatitudeCircle";
        }
    }
    else {
//        mDebug() << "Don't RespectLatitudeCircle";
    }
    

    qreal     x = 0;
    qreal     y = 0;

    // Declare current values.
    bool globeHidesPoint = false;

    // Take the clampToGround property into account
    // For the clampToGround case add the "previous" coordinate before adding any other node. 
    if ( clampToGround && previousAltitude != 0.0 ) {
          q->screenCoordinates( previousCoords, viewport, x, y, globeHidesPoint );
          if ( !globeHidesPoint ) {
            path << QPointF( x, y );
          }
    }

    GeoDataCoordinates previousModifiedCoords( previousCoords );
    if ( clampToGround ) {
        previousModifiedCoords.setAltitude( 0.0 );
    }
    q->screenCoordinates( previousModifiedCoords, viewport, x, y, globeHidesPoint );
    if ( !globeHidesPoint ) {
        path << QPointF( x, y );
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
            lon = lonDiff * t + previousLongitude;
            lat = previousLatitude;
        }
        else {
            // To tessellate along great circles use the 
            // normalized linear interpolation ("NLERP") for latitude and longitude.
            const Quaternion itpos = Quaternion::nlerp( previousCoords.quaternion(), currentCoords.quaternion(), t );
            itpos. getSpherical( lon, lat );
        }

        q->screenCoordinates( GeoDataCoordinates( lon, lat, altitude ), viewport, x, y, globeHidesPoint );

        // No "else" here, as this would not add the current point that is required.
        if ( !globeHidesPoint ) {
            path << QPointF( x, y );
        }
    }


    GeoDataCoordinates currentModifiedCoords( currentCoords );
    if ( clampToGround ) {
        currentModifiedCoords.setAltitude( 0.0 );
    }
    q->screenCoordinates( currentModifiedCoords, viewport, x, y, globeHidesPoint );
    if ( !globeHidesPoint ) {
        path << QPointF( x, y );
    }

    // For the clampToGround case add the "current" coordinate after adding all other nodes. 
    if ( clampToGround && currentCoords.altitude() != 0.0 ) {
          q->screenCoordinates( currentCoords, viewport, x, y, globeHidesPoint );
          if ( !globeHidesPoint ) {
            path << QPointF( x, y );
          }
    }

    return path; 
}


GeoDataCoordinates AbstractProjectionPrivate::findHorizon( const GeoDataCoordinates & previousCoords,
                                                    const GeoDataCoordinates & currentCoords,
                                                    const ViewportParams *viewport,
                                                    TessellationFlags f,
                                                    int recursionCounter ) const
{
    bool currentHide = globeHidesPoint( currentCoords, viewport ) ;

    if ( recursionCounter > 20 ) {
        return currentHide ? previousCoords : currentCoords;
    }
    ++recursionCounter;

    bool followLatitudeCircle = false;

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
        }
        else {
//            mDebug() << "Don't FollowLatitudeCircle";
        }
    }

    qreal  lon = 0.0;
    qreal  lat = 0.0;

    qreal altDiff = currentCoords.altitude() - previousCoords.altitude();

    if ( followLatitudeCircle ) {
        // To tessellate along latitude circles use the
        // linear interpolation of the longitude.
        lon = lonDiff * 0.5 + previousLongitude;
        lat = previousLatitude;
    }
    else {
        // To tessellate along great circles use the
        // normalized linear interpolation ("NLERP") for latitude and longitude.
        const Quaternion itpos = Quaternion::nlerp( previousCoords.quaternion(), currentCoords.quaternion(), 0.5 );
        itpos. getSpherical( lon, lat );
    }

    qreal altitude = previousCoords.altitude() + 0.5 * altDiff;

    GeoDataCoordinates horizonCoords( lon, lat, altitude );

    bool horizonHide = globeHidesPoint( horizonCoords, viewport );

    if ( horizonHide != currentHide ) {
        return findHorizon( horizonCoords, currentCoords, viewport, f, recursionCounter );
    }

    return findHorizon( previousCoords, horizonCoords, viewport, f, recursionCounter );
}


bool AbstractProjectionPrivate::globeHidesPoint( const GeoDataCoordinates &coordinates,
                                          const ViewportParams *viewport ) const
{
    qreal       absoluteAltitude = coordinates.altitude() + EARTH_RADIUS;
    Quaternion  qpos             = coordinates.quaternion();

    qpos.rotateAroundAxis( *( viewport->planetAxisMatrix() ) );

    qreal      pixelAltitude = ( ( viewport->radius() )
                                  / EARTH_RADIUS * absoluteAltitude );
    if ( coordinates.altitude() < 10000 ) {
        // Skip placemarks at the other side of the earth.
        if ( qpos.v[Q_Z] < 0 ) {
            return true;
        }
    }
    else {
        qreal  earthCenteredX = pixelAltitude * qpos.v[Q_X];
        qreal  earthCenteredY = pixelAltitude * qpos.v[Q_Y];
        qreal  radius         = viewport->radius();

        // Don't draw high placemarks (e.g. satellites) that aren't visible.
        if ( qpos.v[Q_Z] < 0
             && ( ( earthCenteredX * earthCenteredX
                    + earthCenteredY * earthCenteredY )
                  < radius * radius ) ) {
            return true;
        }
    }

    return false;
}



/* DEPRECATED */
bool AbstractProjection::screenCoordinates( const GeoDataCoordinates &geopoint, 
                                            const ViewportParams *viewport,
                                            int &x, int &y, bool &globeHidesPoint ) const
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
                                    int &x, int &y ) const
{
    qreal rx = 0.0;
    qreal ry = 0.0;

    bool isVisible = screenCoordinates( lon, lat, viewport, rx, ry );

    x = (int)(rx);
    y = (int)(ry);

    return isVisible;
}
