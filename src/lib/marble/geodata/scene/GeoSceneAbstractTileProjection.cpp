/*
    Copyright 2016 Friedrich W. H. Kossebau  <kossebau@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "GeoSceneAbstractTileProjection.h"

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

}
