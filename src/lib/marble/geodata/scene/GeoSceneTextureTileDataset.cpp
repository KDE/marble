/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/


#include "GeoSceneTextureTileDataset.h"
#include "GeoSceneTypes.h"

namespace Marble
{

GeoSceneTextureTileDataset::GeoSceneTextureTileDataset( const QString& name )
    : GeoSceneTileDataset( name ){
}

const char* GeoSceneTextureTileDataset::nodeType() const
{
    return GeoSceneTypes::GeoSceneTextureTileType;
}

}
