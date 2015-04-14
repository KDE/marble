/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright (C) 2008 Torsten Rahn <rahn@kde.org>

 Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/


#include "GeoSceneTextureTile.h"
#include "GeoSceneTypes.h"

namespace Marble
{

GeoSceneTextureTile::GeoSceneTextureTile( const QString& name )
    : GeoSceneTiled( name ){
}

const char* GeoSceneTextureTile::nodeType() const
{
    return GeoSceneTypes::GeoSceneTextureTileType;
}

GeoDataLatLonBox GeoSceneTextureTile::latLonBox() const
{
    return m_latLonBox;
}

void GeoSceneTextureTile::setLatLonBox( const GeoDataLatLonBox &box )
{
    m_latLonBox = box;
}


}
