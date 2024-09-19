// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#ifndef MARBLE_GEODATANETWORKLINK_H
#define MARBLE_GEODATANETWORKLINK_H

#include "GeoDataFeature.h"
#include "MarbleGlobal.h"

namespace Marble
{

class GeoDataLink;
class GeoDataNetworkLinkPrivate;

/**
 */
class GEODATA_EXPORT GeoDataNetworkLink : public GeoDataFeature
{
public:
    GeoDataNetworkLink();

    GeoDataNetworkLink(const GeoDataNetworkLink &other);

    GeoDataNetworkLink &operator=(const GeoDataNetworkLink &other);

    bool operator==(const GeoDataNetworkLink &other) const;
    bool operator!=(const GeoDataNetworkLink &other) const;

    ~GeoDataNetworkLink() override;

    GeoDataFeature *clone() const override;

    /** Provides type information for downcasting a GeoNode */
    const char *nodeType() const override;

    bool refreshVisibility() const;

    void setRefreshVisibility(bool refreshVisibility);

    bool flyToView() const;

    void setFlyToView(bool flyToView);

    GeoDataLink &link();

    const GeoDataLink &link() const;

    void setLink(const GeoDataLink &link);

private:
    GeoDataNetworkLinkPrivate *const d;
};

}

#endif
