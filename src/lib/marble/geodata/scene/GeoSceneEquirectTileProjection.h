/*
    SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOSCENEEQUIRECTTILEPROJECTION_H
#define MARBLE_GEOSCENEEQUIRECTTILEPROJECTION_H

#include "GeoSceneAbstractTileProjection.h"

namespace Marble
{

/**
 * Converts the x and y indices of tiles to and from geo coordinates.
 * For tiles of maps in Equirectangular projection.
 *
 * Tiles do have the same width and the same height per zoomlevel.
 * The number of tiles per dimension is twice that of the previous lower zoomlevel.
 * The indexing is done in x dimension eastwards, with the first tiles beginning at -180 degree
 * and an x value of 0 and the last tiles ending at +180 degree,
 * in y dimension southwards with the first tiles beginning at +90 degree and a y value of 0
 * and the last tiles ending at -90 degree.
 */
class GEODATA_EXPORT GeoSceneEquirectTileProjection : public GeoSceneAbstractTileProjection
{
public:
    /**
     * @brief Construct a new GeoSceneEquirectTileProjection.
     */
    GeoSceneEquirectTileProjection();

    ~GeoSceneEquirectTileProjection() override;

public:
    /**
     * @copydoc
     */
    GeoSceneAbstractTileProjection::Type type() const override;

    /**
     * @copydoc
     */
    QRect tileIndexes(const GeoDataLatLonBox &latLonBox, int zoomLevel) const override;

    /**
     * @copydoc
     */
    GeoDataLatLonBox geoCoordinates(int zoomLevel, int x, int y) const override;

    using GeoSceneAbstractTileProjection::geoCoordinates;

private:
    Q_DISABLE_COPY(GeoSceneEquirectTileProjection)
};

}

#endif
