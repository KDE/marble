// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef GEODATACHANGE_H
#define GEODATACHANGE_H

#include "GeoDataContainer.h"
#include "geodata_export.h"

namespace Marble
{

class GeoDataChangePrivate;

/**
 */
class GEODATA_EXPORT GeoDataChange : public GeoDataContainer
{
public:
    GeoDataChange();

    GeoDataChange(const GeoDataChange &other);

    ~GeoDataChange() override;

    GeoDataChange &operator=(const GeoDataChange &other);

    bool operator==(const GeoDataChange &other) const;
    bool operator!=(const GeoDataChange &other) const;

    GeoDataFeature *clone() const override;

    /** Provides type information for downcasting a GeoNode */
    const char *nodeType() const override;

private:
    Q_DECLARE_PRIVATE(GeoDataChange)
};

}

#endif
