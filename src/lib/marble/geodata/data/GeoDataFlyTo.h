// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#ifndef MARBLE_GEODATAFLYTO_H
#define MARBLE_GEODATAFLYTO_H

#include "GeoDataTourPrimitive.h"
#include "MarbleGlobal.h"

namespace Marble
{

class GeoDataAbstractView;
class GeoDataFlyToPrivate;

/**
 */
class GEODATA_EXPORT GeoDataFlyTo : public GeoDataTourPrimitive
{
public:
    enum FlyToMode {
        Bounce,
        Smooth
    };

    GeoDataFlyTo();

    GeoDataFlyTo(const GeoDataFlyTo &other);

    GeoDataFlyTo &operator=(const GeoDataFlyTo &other);
    bool operator==(const GeoDataFlyTo &other) const;
    bool operator!=(const GeoDataFlyTo &other) const;

    ~GeoDataFlyTo() override;

    /** Provides type information for downcasting a GeoNode */
    const char *nodeType() const override;

    double duration() const;

    void setDuration(double duration);

    const GeoDataAbstractView *view() const;

    GeoDataAbstractView *view();

    void setView(GeoDataAbstractView *view);

    FlyToMode flyToMode() const;

    void setFlyToMode(const FlyToMode flyToMode);

private:
    GeoDataFlyToPrivate *const d;
};

}

#endif
