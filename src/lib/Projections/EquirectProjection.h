//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007       Inge Wallin  <ingwa@kde.org>
// Copyright 2007-2009  Torsten Rahn  <rahn@kde.org>
//


#ifndef EQUIRECTPROJECTION_H
#define EQUIRECTPROJECTION_H


/** @file
 * This file contains the headers for EquirectProjection.
 *
 * @author Inge Wallin  <inge@lysator.liu.se>
 * @author Torsten Rahn <rahn@kde.org>
 */


#include "AbstractProjection.h"


namespace Marble
{

/**
 * @short A class to implement the Equirectangular projection used by the "Flat Map" view.
 */

class EquirectProjection : public AbstractProjection
{
    // Not a QObject so far because we don't need to send signals.
 public:

    /**
     * @brief Construct a new EquirectProjection.
     */
    EquirectProjection();

    virtual ~EquirectProjection();

    virtual SurfaceType surfaceType() const { return Cylindrical; }

    virtual PreservationType preservationType() const { return NoPreservation; }

    AbstractProjectionHelper *helper();

    /**
     * @brief Get the screen coordinates corresponding to geographical coordinates in the map.
     * @param lon    the lon coordinate of the requested pixel position
     * @param lat    the lat coordinate of the requested pixel position
     * @param x      the x coordinate of the pixel is returned through this parameter
     * @param y      the y coordinate of the pixel is returned through this parameter
     * @return @c true  if the geographical coordinates are visible on the screen
     *         @c false if the geographical coordinates are not visible on the screen
     */
    bool screenCoordinates( qreal lon, qreal lat,
                            const ViewportParams *params,
                            qreal& x, qreal& y );

    bool screenCoordinates( const GeoDataCoordinates &geopoint, 
                            const ViewportParams *params,
                            qreal &x, qreal &y, bool &globeHidesPoint );

    bool screenCoordinates( const GeoDataCoordinates &coordinates,
                            const ViewportParams *viewport,
                            qreal *x, qreal &y, int &pointRepeatNum,
                            const QSizeF& size,
                            bool &globeHidesPoint );

    /**
     * @brief Get the earth coordinates corresponding to a pixel in the map.
     * @param x      the x coordinate of the pixel
     * @param y      the y coordinate of the pixel
     * @param lon    the longitude angle is returned through this parameter
     * @param lat    the latitude angle is returned through this parameter
     * @return @c true  if the pixel (x, y) is within the globe
     *         @c false if the pixel (x, y) is outside the globe, i.e. in space.
     */
    bool geoCoordinates( int x, int y,
                         const ViewportParams *params,
                         qreal& lon, qreal& lat,
                         GeoDataCoordinates::Unit unit = GeoDataCoordinates::Degree );

    GeoDataLatLonAltBox latLonAltBox( const QRect& screenRect,
                                      const ViewportParams *viewport );

    bool mapCoversViewport( const ViewportParams *viewport ) const;

 private:
    //EquirectProjectionPrivate  * const d;
};

}

#endif // EQUIRECTPROJECTION_H
