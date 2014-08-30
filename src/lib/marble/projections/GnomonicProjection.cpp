//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Bernhard Beschow  <bbeschow@cs.tu-berlin.de>
//

// Local
#include "GnomonicProjection.h"
#include "AbstractProjection_p.h"

#include "MarbleDebug.h"

// Marble
#include "ViewportParams.h"
#include "GeoDataPoint.h"
#include "GeoDataLineString.h"
#include "GeoDataCoordinates.h"
#include "MarbleGlobal.h"
#include "AzimuthalProjection_p.h"

#include <qmath.h>

#define SAFE_DISTANCE

namespace Marble
{

class GnomonicProjectionPrivate : public AzimuthalProjectionPrivate
{
  public:
    explicit GnomonicProjectionPrivate( GnomonicProjection * parent );

    // This method tessellates a line segment in a way that the line segment
    // follows great circles. The count parameter specifies the
    // number of nodes generated for the polygon. If the
    // clampToGround flag is added the polygon contains count + 2
    // nodes as the clamped down start and end node get added.

    void tessellateLineSegment(  const GeoDataCoordinates &aCoords,
                                qreal ax, qreal ay,
                                const GeoDataCoordinates &bCoords,
                                qreal bx, qreal by,
                                QVector<QPolygonF*> &polygons,
                                const ViewportParams *viewport,
                                TessellationFlags f = 0 ) const;

    void processTessellation(   const GeoDataCoordinates &previousCoords,
                               const GeoDataCoordinates &currentCoords,
                               int count,
                               QVector<QPolygonF*> &polygons,
                               const ViewportParams *viewport,
                               TessellationFlags f = 0 ) const;

    void crossHorizon( const GeoDataCoordinates & bCoord,
                       QVector<QPolygonF*> &polygons,
                       const ViewportParams *viewport ) const;

    virtual bool lineStringToPolygon( const GeoDataLineString &lineString,
                              const ViewportParams *viewport,
                              QVector<QPolygonF*> &polygons ) const;

    void horizonToPolygon( const ViewportParams *viewport,
                           const GeoDataCoordinates & disappearCoords,
                           const GeoDataCoordinates & reappearCoords,
                           QPolygonF* ) const;

    GeoDataCoordinates findHorizon( const GeoDataCoordinates & previousCoords,
                                    const GeoDataCoordinates & currentCoords,
                                    const ViewportParams *viewport,
                                    TessellationFlags f = 0,
                                    int recursionCounter = 0 ) const;

    bool globeHidesPoint( const GeoDataCoordinates &coordinates,
                          const ViewportParams *viewport ) const;

    Q_DECLARE_PUBLIC( GnomonicProjection )
};

GnomonicProjection::GnomonicProjection()
    : AzimuthalProjection( new GnomonicProjectionPrivate( this ) )
{
    setMinLat( minValidLat() );
    setMaxLat( maxValidLat() );
}

GnomonicProjection::GnomonicProjection( GnomonicProjectionPrivate *dd )
        : AzimuthalProjection( dd )
{
    setMinLat( minValidLat() );
    setMaxLat( maxValidLat() );
}

GnomonicProjection::~GnomonicProjection()
{
}


GnomonicProjectionPrivate::GnomonicProjectionPrivate( GnomonicProjection * parent )
        : AzimuthalProjectionPrivate( parent )
{

}

bool GnomonicProjection::screenCoordinates( const GeoDataCoordinates &coordinates,
                                             const ViewportParams *viewport,
                                             qreal &x, qreal &y, bool &globeHidesPoint ) const
{
    const qreal lambda = coordinates.longitude();
    const qreal phi = coordinates.latitude();
    const qreal lambdaPrime = viewport->centerLongitude();
    const qreal phi1 = viewport->centerLatitude();

    qreal cosC = qSin( phi1 ) * qSin( phi ) + qCos( phi1 ) * qCos( phi ) * qCos( lambda - lambdaPrime );

    // Prevent division by zero
    if (fabs(cosC < 0.0001)) cosC = 0.0001;

    // Let (x, y) be the position on the screen of the placemark..
    x = ( qCos( phi ) * qSin( lambda - lambdaPrime ) ) / cosC;
    y = ( qCos( phi1 ) * qSin( phi ) - qSin( phi1 ) * qCos( phi ) * qCos( lambda - lambdaPrime ) ) / cosC;

    x *= 2 * viewport->radius() / M_PI;
    y *= 2 * viewport->radius() / M_PI;

    const qint64  radius  = viewport->radius();

    // Introduce arteficial 10*radius horizon to avoid projection artefacts
    if (x*x + y*y > 100 * radius * radius) {
        globeHidesPoint = true;
        return false;
    }

    globeHidesPoint = false;

    x += viewport->width() / 2;
    y = viewport->height() / 2 - y;

    // Skip placemarks that are outside the screen area
    if ( x < 0 || x >= viewport->width() || y < 0 || y >= viewport->height() ) {
        return false;
    }

    return true;
}

bool GnomonicProjection::screenCoordinates( const GeoDataCoordinates &coordinates,
                                             const ViewportParams *viewport,
                                             qreal *x, qreal &y,
                                             int &pointRepeatNum,
                                             const QSizeF& size,
                                             bool &globeHidesPoint ) const
{
    pointRepeatNum = 0;
    globeHidesPoint = false;

    bool visible = screenCoordinates( coordinates, viewport, *x, y, globeHidesPoint );

    // Skip placemarks that are outside the screen area
    if ( *x + size.width() / 2.0 < 0.0 || *x >= viewport->width() + size.width() / 2.0
         || y + size.height() / 2.0 < 0.0 || y >= viewport->height() + size.height() / 2.0 )
    {
        return false;
    }

    // This projection doesn't have any repetitions,
    // so the number of screen points referring to the geopoint is one.
    pointRepeatNum = 1;
    return visible;
}


bool GnomonicProjection::geoCoordinates( const int x, const int y,
                                          const ViewportParams *viewport,
                                          qreal& lon, qreal& lat,
                                          GeoDataCoordinates::Unit unit ) const
{
    const qint64  radius  = viewport->radius();
    // Calculate how many degrees are being represented per pixel.
    const qreal rad2Pixel = ( 2 * radius ) / M_PI;
    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();
    const qreal rx = ( - viewport->width()  / 2 + x ) / rad2Pixel;
    const qreal ry = (   viewport->height() / 2 - y ) / rad2Pixel;
    const qreal p = qSqrt( rx*rx + ry*ry );
    const qreal c = qAtan( p );
    const qreal sinc = qSin(c);

    lon = centerLon + qAtan2( rx*sinc , ( p*qCos( centerLat )*qCos( c ) - ry*qSin( centerLat )*sinc  ) );

    while ( lon < -M_PI ) lon += 2 * M_PI;
    while ( lon >  M_PI ) lon -= 2 * M_PI;

    lat = qAsin( qCos(c)*qSin(centerLat) + ry*sinc*qCos(centerLat)/p );

    if ( unit == GeoDataCoordinates::Degree ) {
        lon *= RAD2DEG;
        lat *= RAD2DEG;
    }

    return true;
}

bool GnomonicProjection::mapCoversViewport( const ViewportParams *viewport ) const
{
    return true;
}

bool GnomonicProjection::screenCoordinates( const GeoDataLineString &lineString,
                                                  const ViewportParams *viewport,
                                                  QVector<QPolygonF *> &polygons ) const
{

    Q_D( const GnomonicProjection );
    // Compare bounding box size of the line string with the angularResolution
    // Immediately return if the latLonAltBox is smaller.
    if ( !viewport->resolves( lineString.latLonAltBox() ) ) {
//      mDebug() << "Object too small to be resolved";
        return false;
    }

    d->lineStringToPolygon( lineString, viewport, polygons );
    return true;
}

void GnomonicProjectionPrivate::tessellateLineSegment( const GeoDataCoordinates &aCoords,
                                                qreal ax, qreal ay,
                                                const GeoDataCoordinates &bCoords,
                                                qreal bx, qreal by,
                                                QVector<QPolygonF*> &polygons,
                                                const ViewportParams *viewport,
                                                TessellationFlags f) const
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

        // Let the line segment follow the spherical surface
        // if the distance between the previous point and the current point
        // on screen is too big

        if ( distance > finalTessellationPrecision ) {
            const int tessellatedNodes = qMin<int>( distance / finalTessellationPrecision, maxTessellationNodes );

            processTessellation( aCoords, bCoords,
                                 tessellatedNodes,
                                 polygons,
                                 viewport,
                                 f );
        }
        else {
            crossHorizon( bCoords, polygons, viewport );
        }
#ifdef SAFE_DISTANCE
    }
#endif
}


void GnomonicProjectionPrivate::processTessellation( const GeoDataCoordinates &previousCoords,
                                                    const GeoDataCoordinates &currentCoords,
                                                    int tessellatedNodes,
                                                    QVector<QPolygonF*> &polygons,
                                                    const ViewportParams *viewport,
                                                    TessellationFlags f) const
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
        crossHorizon( currentTessellatedCoords, polygons, viewport );
        previousTessellatedCoords = currentTessellatedCoords;
    }

    // For the clampToGround case add the "current" coordinate after adding all other nodes.
    GeoDataCoordinates currentModifiedCoords( currentCoords );
    if ( clampToGround ) {
        currentModifiedCoords.setAltitude( 0.0 );
    }
    crossHorizon( currentModifiedCoords, polygons, viewport );
}

void GnomonicProjectionPrivate::crossHorizon( const GeoDataCoordinates & bCoord,
                                              QVector<QPolygonF*> &polygons,
                                              const ViewportParams *viewport ) const
{
    qreal x, y;
    bool globeHidesPoint;

    Q_Q( const AbstractProjection );

    q->screenCoordinates( bCoord, viewport, x, y, globeHidesPoint );

    if( !globeHidesPoint ) {
        *polygons.last() << QPointF( x, y );
    }
    else {
        if ( !polygons.last()->isEmpty() ) {
            QPolygonF *path = new QPolygonF;
            polygons.append( path );
        }
    }
}

bool GnomonicProjectionPrivate::lineStringToPolygon( const GeoDataLineString &lineString,
                                              const ViewportParams *viewport,
                                              QVector<QPolygonF *> &polygons ) const
{
    Q_Q( const GnomonicProjection );

    const TessellationFlags f = lineString.tessellationFlags();

    qreal x = 0;
    qreal y = 0;
    bool globeHidesPoint = false;

    qreal previousX = -1.0;
    qreal previousY = -1.0;
    bool previousGlobeHidesPoint = false;

    qreal horizonX = -1.0;
    qreal horizonY = -1.0;

    polygons.append( new QPolygonF );

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

    GeoDataLineString::ConstIterator itBegin = lineString.constBegin();
    GeoDataLineString::ConstIterator itEnd = lineString.constEnd();

    bool processingLastNode = false;

    // We use a while loop to be able to cover linestrings as well as linear rings:
    // Linear rings require to tessellate the path from the last node to the first node
    // which isn't really convenient to achieve with a for loop ...

    const bool isLong = lineString.size() > 50;
    const int maximumDetail = ( viewport->radius() > 5000 ) ? 5 :
                              ( viewport->radius() > 2500 ) ? 4 :
                              ( viewport->radius() > 1000 ) ? 3 :
                              ( viewport->radius() >  600 ) ? 2 :
                              ( viewport->radius() >   50 ) ? 1 :
                                                              0;

    while ( itCoords != itEnd )
    {

        // Optimization for line strings with a big amount of nodes
        bool skipNode = itCoords != itBegin && isLong && !processingLastNode &&
                ( (*itCoords).detail() > maximumDetail
                  || viewport->resolves( *itPreviousCoords, *itCoords ) );

        if ( !skipNode ) {

            q->screenCoordinates( *itCoords, viewport, x, y, globeHidesPoint );

            // Initializing variables that store the values of the previous iteration
            if ( !processingLastNode && itCoords == itBegin ) {
                previousGlobeHidesPoint = globeHidesPoint;
                itPreviousCoords = itCoords;
                previousX = x;
                previousY = y;
            }

            // Check for the "horizon case" (which is present e.g. for the spherical projection
            const bool isAtHorizon = ( globeHidesPoint || previousGlobeHidesPoint ) &&
                                     ( globeHidesPoint !=  previousGlobeHidesPoint );

            if ( isAtHorizon ) {
                // Handle the "horizon case"
                horizonCoords = findHorizon( *itPreviousCoords, *itCoords, viewport, f );

                if ( lineString.isClosed() ) {
                    if ( horizonPair ) {
                        horizonToPolygon( viewport, horizonDisappearCoords, horizonCoords, polygons.last() );
                        horizonPair = false;
                    }
                    else {
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

                q->screenCoordinates( horizonCoords, viewport, horizonX, horizonY );

                // If the line appears on the visible half we need
                // to add an interpolated point at the horizon as the previous point.
                if ( previousGlobeHidesPoint ) {
                    *polygons.last() << QPointF( horizonX, horizonY );
                }
            }

            // This if-clause contains the section that tessellates the line
            // segments of a linestring. If you are about to learn how the code of
            // this class works you can safely ignore this section for a start.

            if ( lineString.tessellate() /* && ( isVisible || previousIsVisible ) */ ) {

                if ( !isAtHorizon ) {

                    tessellateLineSegment( *itPreviousCoords, previousX, previousY,
                                           *itCoords, x, y,
                                           polygons, viewport,
                                           f );

                }
                else {
                    // Connect the interpolated  point at the horizon with the
                    // current or previous point in the line.
                    if ( previousGlobeHidesPoint ) {
                        tessellateLineSegment( horizonCoords, horizonX, horizonY,
                                               *itCoords, x, y,
                                               polygons, viewport,
                                               f );
                    }
                    else {
                        tessellateLineSegment( *itPreviousCoords, previousX, previousY,
                                               horizonCoords, horizonX, horizonY,
                                               polygons, viewport,
                                               f );
                    }
                }
            }
            else {
                if ( !globeHidesPoint ) {
                    *polygons.last() << QPointF( x, y );
                }
                else {
                    if ( !previousGlobeHidesPoint && isAtHorizon ) {
                        *polygons.last() << QPointF( horizonX, horizonY );
                    }
                }
            }

            if ( globeHidesPoint ) {
                if (   !previousGlobeHidesPoint
                    && !lineString.isClosed()
                    ) {
                    polygons.append( new QPolygonF );
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
        horizonToPolygon( viewport, horizonCoords, horizonOrphanCoords, polygons.last() );
    }

    if ( polygons.last()->size() <= 1 ){
        polygons.pop_back(); // Clean up "unused" empty polygon instances
    }

    return polygons.isEmpty();
}

void GnomonicProjectionPrivate::horizonToPolygon( const ViewportParams *viewport,
                                           const GeoDataCoordinates & disappearCoords,
                                           const GeoDataCoordinates & reappearCoords,
                                           QPolygonF * polygon ) const
{
    qreal x, y;

    const qreal imageHalfWidth  = viewport->width() / 2;
    const qreal imageHalfHeight = viewport->height() / 2;

    bool dummyGlobeHidesPoint = false;

    Q_Q( const GnomonicProjection );
    // Calculate the angle of the position vectors of both coordinates
    q->screenCoordinates( disappearCoords, viewport, x, y, dummyGlobeHidesPoint );
    qreal alpha = atan2( y - imageHalfHeight,
                         x - imageHalfWidth );

    q->screenCoordinates( reappearCoords, viewport, x, y, dummyGlobeHidesPoint );
    qreal beta =  atan2( y - imageHalfHeight,
                         x - imageHalfWidth );

    // Calculate the difference between both
    qreal diff = GeoDataCoordinates::normalizeLon( beta - alpha );

    qreal sgndiff = diff < 0 ? -1 : 1;

    const qreal arcradius = 10 * viewport->radius();
    const int itEnd = fabs(diff * RAD2DEG);

    // Create a polygon that resembles an arc between the two position vectors
    for ( int it = 1; it <= itEnd; ++it ) {
        const qreal angle = alpha + DEG2RAD * sgndiff * it;
        const qreal itx = imageHalfWidth  +  arcradius * cos( angle );
        const qreal ity = imageHalfHeight +  arcradius * sin( angle );
        *polygon << QPointF( itx, ity );
    }
}


GeoDataCoordinates GnomonicProjectionPrivate::findHorizon( const GeoDataCoordinates & previousCoords,
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
        qreal previousSign = previousLongitude > 0 ? 1 : -1;

        qreal currentLongitude = 0.0;
        qreal currentLatitude = 0.0;
        currentCoords.geoCoordinates( currentLongitude, currentLatitude );
        qreal currentSign = currentLongitude > 0 ? 1 : -1;

        if ( previousLatitude == currentLatitude ) {
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


bool GnomonicProjectionPrivate::globeHidesPoint( const GeoDataCoordinates &coordinates,
                                          const ViewportParams *viewport ) const
{
    bool globeHidesPoint;
    qreal dummyX, dummyY;

    Q_Q( const GnomonicProjection );
    q->screenCoordinates(coordinates, viewport, dummyX, dummyY, globeHidesPoint);
    return globeHidesPoint;
}

QPainterPath GnomonicProjection::mapShape( const ViewportParams *viewport ) const
{
    // Convenience variables
    int  width  = viewport->width();
    int  height = viewport->height();

    // Cover the whole screen
    QPainterPath mapShape;
    mapShape.addRect(
                    0,
                    0,
                    width,
                    height );

    return mapShape;
}

}
