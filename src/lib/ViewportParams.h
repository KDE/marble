//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#ifndef MARBLE_VIEWPORTPARAMS_H
#define MARBLE_VIEWPORTPARAMS_H


/** @file
 * This file contains the headers for ViewportParams.
 * 
 * @author Inge Wallin  <inge@lysator.liu.se>
 */

#include <QtCore/QSize>
#include <QtGui/QPainterPath>

#include "GeoDataLatLonAltBox.h"

#include "Quaternion.h"
#include "MarbleGlobal.h"
#include "marble_export.h"

class QPolygonF;

namespace Marble
{

class AbstractProjection;
class ViewportParamsPrivate;

/** 
 * @short A public class that controls what is visible in the viewport of a Marble map.
 *
 */

class MARBLE_EXPORT ViewportParams
{
 public:
    ViewportParams( );
    ~ViewportParams();

    // Getters and setters
    Projection projection() const;
    const AbstractProjection *currentProjection() const;
    void setProjection(Projection newProjection);

    int polarity() const;

    GeoDataLatLonAltBox& viewLatLonAltBox() const;

    GeoDataLatLonAltBox latLonAltBox( const QRect &screenRect ) const;

    // Calculates an educated guess for the average angle in radians covered per pixel.
    // Given a certain resolution it doesn't make much sense
    // - to display an object that covers an angle that is smaller than that.
    // - to display two points as distinct points if they are separated by a 
    //   an angular distance that is smaller. Instead only one point should be shown.
    // So this method helps to filter out details.
    // It's somewhat related to http://en.wikipedia.org/wiki/Angular_resolution

    qreal angularResolution() const;

    // Determines whether a geographical feature is big enough so that it should 
    // represent a single point on the screen already.
    // See angularResolution()

    bool resolves ( const GeoDataLatLonBox &latLonBox ) const;

    bool resolves ( const GeoDataLatLonAltBox &latLonAltBox ) const;

    // Determines whether two points are located enough apart so that it makes 
    // sense to display them as distinct points. If this is not the case
    // calculation and drawing of one point can be skipped as only a single
    // point will be displayed on the screen.
    
    bool resolves ( const GeoDataCoordinates &coord1, const GeoDataCoordinates &coord2 ) const;

    int  radius() const;

    /**
     * @brief Change the radius of the planet
     * @param radius Size of the planet radius in pixel. Non-positive values are ignored.
     */
    void setRadius(int radius);

    void centerOn( qreal lon, qreal lat );

    Quaternion planetAxis() const;
    const matrix * planetAxisMatrix() const;

    int width()  const;
    int height() const;
    QSize size() const;

    void setWidth(int newWidth);
    void setHeight(int newHeight);
    void setSize(QSize newSize);

    qreal centerLongitude() const;
    qreal centerLatitude() const;
    MARBLE_DEPRECATED( void centerCoordinates( qreal &centerLon, qreal &centerLat ) const );

    /**
     * @brief Get the screen coordinates corresponding to geographical coordinates in the map.
     * @param lon    the lon coordinate of the requested pixel position in radians
     * @param lat    the lat coordinate of the requested pixel position in radians
     * @param x      the x coordinate of the pixel is returned through this parameter
     * @param y      the y coordinate of the pixel is returned through this parameter
     * @return @c true  if the geographical coordinates are visible on the screen
     *         @c false if the geographical coordinates are not visible on the screen
     *
     * @see ViewportParams
     */
    bool screenCoordinates( const qreal lon, const qreal lat,
                            qreal &x, qreal &y ) const;

    /**
     * @brief Get the screen coordinates corresponding to geographical coordinates in the map.
     *
     * @param geopoint the point on earth, including altitude, that we want the coordinates for.
     * @param x      the x coordinate of the pixel is returned through this parameter
     * @param y      the y coordinate of the pixel is returned through this parameter
     * @param globeHidesPoint  whether the point gets hidden on the far side of the earth
     *
     * @return @c true  if the geographical coordinates are visible on the screen
     *         @c false if the geographical coordinates are not visible on the screen
     *
     * @see ViewportParams
     */
    bool screenCoordinates( const GeoDataCoordinates &geopoint,
                            qreal &x, qreal &y,
                            bool &globeHidesPoint ) const;

    // Will just call the virtual version with a dummy globeHidesPoint.
    bool screenCoordinates( const GeoDataCoordinates &geopoint,
                            qreal &x, qreal &y ) const;

    bool screenCoordinates( const GeoDataCoordinates &geopoint,
                            QPointF &screenpoint ) const;

    /**
     * @brief Get the coordinates of screen points for geographical coordinates in the map.
     *
     * @param coordinates the point on earth, including altitude, that we want the coordinates for.
     * @param x      the x coordinates of the pixels are returned through this parameter
     * @param y      the y coordinate of the pixel is returned through this parameter
     * @param pointRepeatNum      the amount of times that a single geographical
                                  point gets represented on the map
     * @param globeHidesPoint  whether the point gets hidden on the far side of the earth
     *
     * @return @c true  if the geographical coordinates are visible on the screen
     *         @c false if the geographical coordinates are not visible on the screen
     *
     * @see ViewportParams
     */
    bool screenCoordinates( const GeoDataCoordinates &coordinates,
                            qreal *x, qreal &y, int &pointRepeatNum,
                            bool &globeHidesPoint ) const;

    bool screenCoordinates( const GeoDataCoordinates &coordinates,
                            qreal *x, qreal &y, int &pointRepeatNum,
                            const QSizeF& size,
                            bool &globeHidesPoint ) const;


    bool screenCoordinates( const GeoDataLineString &lineString,
                            QVector<QPolygonF*> &polygons ) const;

    /**
     * @brief Get the earth coordinates corresponding to a pixel in the map.
     * @param x      the x coordinate of the pixel
     * @param y      the y coordinate of the pixel
     * @param lon    the longitude angle is returned through this parameter
     * @param lat    the latitude angle is returned through this parameter
     * @param unit   the unit of the angles for lon and lat.
     * @return @c true  if the pixel (x, y) is within the globe
     *         @c false if the pixel (x, y) is outside the globe, i.e. in space.
     */
    bool geoCoordinates( const int x, const int y,
                         qreal &lon, qreal &lat,
                         GeoDataCoordinates::Unit unit = GeoDataCoordinates::Degree ) const;

    bool mapCoversViewport() const;

    QPainterPath mapShape() const;

    QRegion mapRegion() const;

    /**
      * @return The current point of focus, e.g. the point that is not moved
      * when changing the zoom level. If not set, it defaults to the
      * center point.
      * @see centerCoordinates setFocusPoint resetFocusPoint
      */
    GeoDataCoordinates focusPoint() const;

    /**
      * @brief Change the point of focus, overridding any previously set focus point.
      * @param focusPoint New focus point
      * @see focusPoint resetFocusPoint
      */
    void setFocusPoint(const GeoDataCoordinates &focusPoint);

    /**
      * @brief Invalidate any focus point set with @ref setFocusPoint.
      * @see focusPoint setFocusPoint
      */
    void resetFocusPoint();

 private:
    Q_DISABLE_COPY( ViewportParams )
    ViewportParamsPrivate * const d;
};

}

#endif
