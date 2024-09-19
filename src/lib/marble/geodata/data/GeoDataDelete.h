// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef GEODATADELETE_H
#define GEODATADELETE_H

#include "GeoDataContainer.h"
#include "geodata_export.h"

namespace Marble
{

class GeoDataDeletePrivate;

/**
 */
class GEODATA_EXPORT GeoDataDelete : public GeoDataContainer
{
public:
    GeoDataDelete();

    GeoDataDelete(const GeoDataDelete &other);

    ~GeoDataDelete() override;

    GeoDataDelete &operator=(const GeoDataDelete &other);

    bool operator==(const GeoDataDelete &other) const;
    bool operator!=(const GeoDataDelete &other) const;

    GeoDataFeature *clone() const override;

    /** Provides type information for downcasting a GeoNode */
    const char *nodeType() const override;

private:
    Q_DECLARE_PRIVATE(GeoDataDelete)
};

}

#endif
