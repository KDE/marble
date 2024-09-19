// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2007-2012 Torsten Rahn <rahn@kde.org>
//

#ifndef MARBLE_SPHERICALPROJECTION_H
#define MARBLE_SPHERICALPROJECTION_H

/** @file
 * This file contains the headers for SphericalProjection.
 *
 * @author Inge Wallin  <inge@lysator.liu.se>
 * @author Torsten Rahn <rahn@kde.org>
 */

#include "AbstractProjection.h"
#include "AzimuthalProjection.h"

namespace Marble
{

class SphericalProjectionPrivate;

/**
 * @short A class to implement the spherical projection used by the "Globe" view.
 */

class SphericalProjection : public AzimuthalProjection
{
    // Not a QObject so far because we don't need to send signals.
public:
    /**
     * @brief Construct a new SphericalProjection.
     */
    SphericalProjection();

    ~SphericalProjection() override;

    /**
     * @brief Returns the user-visible name of the projection.
     */
    QString name() const override;

    /**
     * @brief Returns a short user description of the projection
     * that can be used in tooltips or dialogs.
     */
    QString description() const override;

    /**
     * @brief Returns an icon for the projection.
     */
    QIcon icon() const override;

    /**
     * @brief Get the screen coordinates corresponding to geographical coordinates in the map.
     * @param coordinates  the coordinates of the requested pixel position
     * @param params the viewport parameters
     * @param x      the x coordinate of the pixel is returned through this parameter
     * @param y      the y coordinate of the pixel is returned through this parameter
     * @param globeHidesPoint whether the globe hides the point
     * @return @c true  if the geographical coordinates are visible on the screen
     *         @c false if the geographical coordinates are not visible on the screen
     */
    bool screenCoordinates(const GeoDataCoordinates &coordinates, const ViewportParams *params, qreal &x, qreal &y, bool &globeHidesPoint) const override;

    bool screenCoordinates(const GeoDataCoordinates &coordinates,
                           const ViewportParams *viewport,
                           qreal *x,
                           qreal &y,
                           int &pointRepeatNum,
                           const QSizeF &size,
                           bool &globeHidesPoint) const override;

    using AbstractProjection::screenCoordinates;

    /**
     * @brief Get the earth coordinates corresponding to a pixel in the map.
     * @param x      the x coordinate of the pixel
     * @param y      the y coordinate of the pixel
     * @param params parameters of the viewport
     * @param lon    the longitude angle is returned through this parameter
     * @param lat    the latitude angle is returned through this parameter
     * @param unit   the unit
     * @return @c true  if the pixel (x, y) is within the globe
     *         @c false if the pixel (x, y) is outside the globe, i.e. in space.
     */
    bool geoCoordinates(const int x,
                        const int y,
                        const ViewportParams *params,
                        qreal &lon,
                        qreal &lat,
                        GeoDataCoordinates::Unit unit = GeoDataCoordinates::Degree) const override;

protected:
    explicit SphericalProjection(SphericalProjectionPrivate *dd);

private:
    Q_DECLARE_PRIVATE(SphericalProjection)
    Q_DISABLE_COPY(SphericalProjection)
};

}

#endif
