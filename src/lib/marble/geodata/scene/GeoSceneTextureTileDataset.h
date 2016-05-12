/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright (C) 2008 Torsten Rahn <rahn@kde.org>

 Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#ifndef MARBLE_GEOSCENETEXTURETILEDATASET_H
#define MARBLE_GEOSCENETEXTURETILEDATASET_H

#include "GeoSceneTileDataset.h"
#include "GeoDataLatLonBox.h"

namespace Marble
{

class GeoSceneTextureTileDataset : public GeoSceneTileDataset
{
 public:

    explicit GeoSceneTextureTileDataset( const QString& name );

    virtual const char* nodeType() const;

    /**
     * @brief set bounds for the texture. Tiles outside of these bounds won't be searched in this texture.
     * Null box means no bounds.
     */
    void setLatLonBox( const GeoDataLatLonBox &box );
    GeoDataLatLonBox latLonBox() const;

 private:
    GeoDataLatLonBox m_latLonBox;
};

}

#endif // MARBLE_GEOSCENETEXTURETILEDATASET_H
