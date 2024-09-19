// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef MARBLE_GEODATAANIMATEDUPDATE_H
#define MARBLE_GEODATAANIMATEDUPDATE_H

#include "GeoDataTourPrimitive.h"
#include "MarbleGlobal.h"

namespace Marble
{

class GeoDataAnimatedUpdatePrivate;
class GeoDataUpdate;

/**
 */
class GEODATA_EXPORT GeoDataAnimatedUpdate : public GeoDataTourPrimitive
{
public:
    GeoDataAnimatedUpdate();
    GeoDataAnimatedUpdate(const GeoDataAnimatedUpdate &other);
    GeoDataAnimatedUpdate &operator=(const GeoDataAnimatedUpdate &other);
    bool operator==(const GeoDataAnimatedUpdate &other) const;
    bool operator!=(const GeoDataAnimatedUpdate &other) const;
    ~GeoDataAnimatedUpdate() override;

    /** Provides type information for downcasting a GeoNode */
    const char *nodeType() const override;

    double duration() const;
    void setDuration(double duration);

    double delayedStart() const;
    void setDelayedStart(double delayedStart);

    const GeoDataUpdate *update() const;
    GeoDataUpdate *update();
    void setUpdate(GeoDataUpdate *update);

private:
    GeoDataAnimatedUpdatePrivate *const d;
};

}

#endif
