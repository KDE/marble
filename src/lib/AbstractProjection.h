//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef ABSTRACTPROJECTION_H
#define ABSTRACTPROJECTION_H


/** @file
 * This file contains the headers for AbstractProjection.
 *
 * @author Inge Wallin  <inge@lysator.liu.se>
 */


#include "GeoDataPoint.h"

class ViewportParams;


/**
 * @short A base class for all projections in Marble.
 */

class AbstractProjection
{
    // Not a QObject so far because we don't need to send signals.
 public:

    /**
     * @brief Construct a new AbstractProjection.
     */
    explicit AbstractProjection();

    virtual ~AbstractProjection();

    /**
     * @brief Get the screen coordinates corresponding to geographical coordinates in the map.
     * @param lon    the lon coordinate of the requested pixel position in radians
     * @param lat    the lat coordinate of the requested pixel position in radians
     * @param params the viewport parametersn
     * @param x      the x coordinate of the pixel is returned through this parameter
     * @param y      the y coordinate of the pixel is returned through this parameter
     * @return @c true  if the geographical coordinates are visible on the screen
     *         @c false if the geographical coordinates are not visible on the screen
     *
     * @see ViewportParams
     */
    virtual bool screenCoordinates( const double lon, const double lat,
                                    const ViewportParams *params,
                                    int& x, int& y ) = 0;

    /**
     * @brief Get the screen coordinates corresponding to geographical coordinates in the map.
     * @param geopoint the point on earth, including altitude, that we want the coordinates for.
     * @param params the viewport parameters
     * @param planetAxisMatrix The matrix describing the current rotation of the globe
     * @param x      the x coordinate of the pixel is returned through this parameter
     * @param y      the y coordinate of the pixel is returned through this parameter
     * @return @c true  if the geographical coordinates are visible on the screen
     *         @c false if the geographical coordinates are not visible on the screen
     *
     * @see ViewportParams
     */
    virtual bool screenCoordinates( const GeoDataPoint &geopoint, 
                                    const ViewportParams *params,
                                    const matrix &planetAxisMatrix,
                                    int &x, int &y ) = 0;

    /**
     * @brief Get the earth coordinates corresponding to a pixel in the map.
     * @param x      the x coordinate of the pixel
     * @param y      the y coordinate of the pixel
     * @param params the viewport parameters
     * @param lon    the longitude angle is returned through this parameter
     * @param lat    the latitude angle is returned through this parameter
     * @param unit   the unit of the angles for lon and lat.
     * @return @c true  if the pixel (x, y) is within the globe
     *         @c false if the pixel (x, y) is outside the globe, i.e. in space.
     */
    virtual bool geoCoordinates( const int x, const int y,
                                 const ViewportParams *params,
                                 double& lon, double& lat,
                                 GeoDataPoint::Unit unit = GeoDataPoint::Degree ) = 0;

    /**
     * @brief Get a quaternion representing a point on the earth corresponding to a pixel in the map.
     * @param x  the x coordinate of the pixel
     * @param y  the y coordinate of the pixel
     * @param q  the out parameter where the result is returned
     * @return @c true  if the pixel (x, y) is within the globe
     *         @c false if the pixel (x, y) is outside the globe, i.e. in space
     */
    virtual bool geoCoordinates( int x, int y,
                                 const ViewportParams *params,
                                 Quaternion &q ) = 0;

 private:
    //AbstractProjectionPrivate  * const d;
};


#endif // ABSTRACTPROJECTION_H
