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
#include "GeoDataLineString.h"
#include "GeoDataCoordinates.h"
#include "ViewportParams.h"

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
        : AbstractProjectionPrivate( parent )
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

    QVector<GeoDataLineString*> lineStrings;

    if (
         ( !traversablePoles() && lineString.latLonAltBox().containsPole( AnyPole ) ) ||
         ( lineString.latLonAltBox().crossesDateLine() )
       ) {
        // We correct for Poles and DateLines:
        lineStrings = lineString.toRangeCorrected();

        foreach ( GeoDataLineString * itLineString, lineStrings ) {
            QVector<QPolygonF *> subPolygons;

            d->lineStringToPolygon( *itLineString, viewport, subPolygons );
            polygons << subPolygons;
        }
    }
    else {
        d->lineStringToPolygon( lineString, viewport, polygons );
    }

    return polygons.isEmpty();
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

    polygons.append( new QPolygonF );

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
        bool skipNode = itCoords != itBegin && isLong && !processingLastNode &&
                        viewport->resolves( *itPreviousCoords, *itCoords );

        if ( !skipNode ) {

            previousCoords = *itPreviousCoords;
            currentCoords  = *itCoords;

            Q_Q( const CylindricalProjection );

            q->screenCoordinates( currentCoords, viewport, x, y );

            // Initializing variables that store the values of the previous iteration
            if ( !processingLastNode && itCoords == itBegin ) {
                itPreviousCoords = itCoords;
                previousX = x;
                previousY = y;
            }

            // This if-clause contains the section that tessellates the line
            // segments of a linestring. If you are about to learn how the code of
            // this class works you can safely ignore this section for a start.

            if ( lineString.tessellate() ) {

                tessellateLineSegment( previousCoords, previousX, previousY,
                                           currentCoords, x, y,
                                           polygons, viewport,
                                           f );
            }

            else {
                // special case for polys which cross dateline but have no Tesselation Flag
                // the expected rendering is a screen coordinates straight line between
                // points, but in projections with repeatX things are not smooth
                // we need to split polygons and use both sides of the repeated point
                crossDateLine( previousCoords, currentCoords, polygons, viewport );
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

        if ( itCoords == itEnd  && lineString.isClosed() ) {
            itCoords = itBegin;
            processingLastNode = true;
        }
    }

    if ( polygons.last()->size() <= 1 ){
        polygons.pop_back(); // Clean up "unused" empty polygon instances
    }

    repeatPolygons( viewport, polygons );

    return polygons.isEmpty();
}

void CylindricalProjectionPrivate::repeatPolygons( const ViewportParams *viewport,
                                                QVector<QPolygonF *> &polygons ) const
{
    Q_Q( const CylindricalProjection );

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

}

