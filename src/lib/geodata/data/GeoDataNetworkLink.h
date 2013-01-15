//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//


#ifndef MARBLE_GEODATANETWORKLINK_H
#define MARBLE_GEODATANETWORKLINK_H

#include "GeoDataFeature.h"
#include "GeoDataLink.h"
#include "MarbleGlobal.h"

namespace Marble {

class GeoDataNetworkLinkPrivate;

class GEODATA_EXPORT GeoDataNetworkLink: public GeoDataFeature
{
public:
    GeoDataNetworkLink();

    GeoDataNetworkLink( const GeoDataNetworkLink &other );

    GeoDataNetworkLink& operator=( const GeoDataNetworkLink &other );

    ~GeoDataNetworkLink();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    bool refreshVisibility() const;

    void setRefreshVisibility( bool refreshVisibility );

    bool flyToView() const;

    void setFlyToView(bool flyToView);

    GeoDataLink& link();

    const GeoDataLink& link() const;

    void setLink(const GeoDataLink &link);

private:
    GeoDataNetworkLinkPrivate* const d;
};

}

#endif
