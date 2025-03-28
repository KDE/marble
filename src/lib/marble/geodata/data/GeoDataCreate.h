// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef GEODATACREATE_H
#define GEODATACREATE_H

#include "GeoDataContainer.h"
#include "geodata_export.h"

namespace Marble
{

class GeoDataCreatePrivate;

/**
 */
class GEODATA_EXPORT GeoDataCreate : public GeoDataContainer
{
public:
    GeoDataCreate();

    GeoDataCreate(const GeoDataCreate &other);

    ~GeoDataCreate() override;

    GeoDataCreate &operator=(const GeoDataCreate &other);

    bool operator==(const GeoDataCreate &other) const;
    bool operator!=(const GeoDataCreate &other) const;

    GeoDataFeature *clone() const override;

    /** Provides type information for downcasting a GeoNode */
    const char *nodeType() const override;

private:
    Q_DECLARE_PRIVATE(GeoDataCreate)
};

}

#endif
