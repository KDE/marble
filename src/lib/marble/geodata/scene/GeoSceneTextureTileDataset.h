/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>

    SPDX-License-Identifier: LGPL-2.1-or-later
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

    const char* nodeType() const override;

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
