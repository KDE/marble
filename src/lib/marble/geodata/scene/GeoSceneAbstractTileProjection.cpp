/*
    SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GeoSceneAbstractTileProjection.h"

#include "GeoDataLatLonBox.h"
#include <TileId.h>

namespace Marble
{

class GeoSceneAbstractTileProjectionPrivate
{
public:
    GeoSceneAbstractTileProjectionPrivate();

public:
    int levelZeroColumns;
    int levelZeroRows;
};

GeoSceneAbstractTileProjectionPrivate::GeoSceneAbstractTileProjectionPrivate()
    : levelZeroColumns(1)
    , levelZeroRows(1)
{
}

GeoSceneAbstractTileProjection::GeoSceneAbstractTileProjection()
    : d_ptr(new GeoSceneAbstractTileProjectionPrivate())
{
}

GeoSceneAbstractTileProjection::~GeoSceneAbstractTileProjection()
{
}

int GeoSceneAbstractTileProjection::levelZeroColumns() const
{
    return d_ptr->levelZeroColumns;
}

void GeoSceneAbstractTileProjection::setLevelZeroColumns(int levelZeroColumns)
{
    d_ptr->levelZeroColumns = levelZeroColumns;
}

int GeoSceneAbstractTileProjection::levelZeroRows() const
{
    return d_ptr->levelZeroRows;
}

void GeoSceneAbstractTileProjection::setLevelZeroRows(int levelZeroRows)
{
    d_ptr->levelZeroRows = levelZeroRows;
}

GeoDataLatLonBox GeoSceneAbstractTileProjection::geoCoordinates(const TileId &tileId) const
{
    return geoCoordinates(tileId.zoomLevel(), tileId.x(), tileId.y());
}

}
