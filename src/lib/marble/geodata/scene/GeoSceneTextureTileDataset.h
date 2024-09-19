/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef MARBLE_GEOSCENETEXTURETILEDATASET_H
#define MARBLE_GEOSCENETEXTURETILEDATASET_H

#include "GeoSceneTileDataset.h"

namespace Marble
{

class GEODATA_EXPORT GeoSceneTextureTileDataset : public GeoSceneTileDataset
{
public:
    explicit GeoSceneTextureTileDataset(const QString &name);

    const char *nodeType() const override;
};

}

#endif // MARBLE_GEOSCENETEXTURETILEDATASET_H
