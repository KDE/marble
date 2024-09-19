/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef MARBLE_GEOSCENEVECTORTILEDATASET_H
#define MARBLE_GEOSCENEVECTORTILEDATASET_H

#include "GeoSceneTileDataset.h"

namespace Marble
{

class GeoSceneVectorTileDataset : public GeoSceneTileDataset
{
public:
    explicit GeoSceneVectorTileDataset(const QString &name);

    const char *nodeType() const override;
};

}

#endif // MARBLE_GEOSCENEVECTORTILEDATASET_H
