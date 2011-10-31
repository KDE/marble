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

#include "GeoDataLatLonAltBox.h"

#include "Quaternion.h"
#include "global.h"
#include "marble_export.h"

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

    GeoDataLatLonAltBox viewLatLonAltBox() const;

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
    void setRadius(int newRadius);

    bool  globeCoversViewport() const;

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

    bool  mapCoversViewport() const;

    /**
      * @return The current point of focus, e.g. the point that is not moved
      * when changing the zoom level. If not set, it defaults to the
      * center point.
      * @see centerCoordinates setFocusPoint resetFocusPoint focusPointIsCenter
      */
    GeoDataCoordinates focusPoint() const;

    /**
      * @brief Change the point of focus, overridding any previously set focus point.
      * @param focusPoint New focus point
      * @see focusPoint resetFocusPoint focusPointIsCenter
      */
    void setFocusPoint(const GeoDataCoordinates &focusPoint);

    /**
      * @brief Invalidate any focus point set with @ref setFocusPoint.
      * @see focusPoint setFocusPoint focusPointIsCenter
      */
    void resetFocusPoint();

 private:
    Q_DISABLE_COPY( ViewportParams )
    ViewportParamsPrivate * const d;
};

}

#endif
