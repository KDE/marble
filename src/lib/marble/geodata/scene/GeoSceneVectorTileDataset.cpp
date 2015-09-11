/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright (C) 2008 Torsten Rahn <rahn@kde.org>

 Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "GeoSceneVectorTileDataset.h"
#include "GeoSceneTypes.h"

namespace Marble
{

GeoSceneVectorTileDataset::GeoSceneVectorTileDataset( const QString& name )
    : GeoSceneTileDataset( name ){
}

const char* GeoSceneVectorTileDataset::nodeType() const
{
    return GeoSceneTypes::GeoSceneVectorTileType;
}

}

