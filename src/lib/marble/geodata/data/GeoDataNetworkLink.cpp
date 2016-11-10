//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#include "GeoDataNetworkLink.h"

#include "GeoDataTypes.h"
#include "GeoDataLink.h"

namespace Marble {

class GeoDataNetworkLinkPrivate
{
public:
    bool m_refreshVisibility;

    bool m_flyToView;

    GeoDataLink m_link;

    GeoDataNetworkLinkPrivate();
};

GeoDataNetworkLinkPrivate::GeoDataNetworkLinkPrivate() :
    m_refreshVisibility(false), m_flyToView(false)
{
    // nothing to do
}

GeoDataNetworkLink::GeoDataNetworkLink() : d( new GeoDataNetworkLinkPrivate )
{
    // nothing to do
}

GeoDataNetworkLink::GeoDataNetworkLink( const Marble::GeoDataNetworkLink &other ) :
    GeoDataFeature( other ), d( new GeoDataNetworkLinkPrivate( *other.d ) )
{
    // nothing to do
}

GeoDataNetworkLink &GeoDataNetworkLink::operator=( const GeoDataNetworkLink &other )
{
    GeoDataFeature::operator=( other );
    *d = *other.d;
    return *this;
}

bool GeoDataNetworkLink::operator==( const GeoDataNetworkLink &other ) const
{
    return equals( other) &&
           d->m_refreshVisibility == other.d->m_refreshVisibility &&
           d->m_link == other.d->m_link &&
           d->m_flyToView == other.d->m_flyToView;
}

bool GeoDataNetworkLink::operator!=( const GeoDataNetworkLink &other ) const
{
    return !this->operator==( other );
}

GeoDataNetworkLink::~GeoDataNetworkLink()
{
    delete d;
}

GeoDataFeature * GeoDataNetworkLink::clone() const
{
    return new GeoDataNetworkLink(*this);
}


const char *GeoDataNetworkLink::nodeType() const
{
    return GeoDataTypes::GeoDataNetworkLinkType;
}

bool GeoDataNetworkLink::refreshVisibility() const
{
    return d->m_refreshVisibility;
}

void GeoDataNetworkLink::setRefreshVisibility( bool refreshVisibility )
{
    d->m_refreshVisibility = refreshVisibility;
}

bool GeoDataNetworkLink::flyToView() const
{
    return d->m_flyToView;
}

void GeoDataNetworkLink::setFlyToView( bool flyToView)
{
    d->m_flyToView = flyToView;
}

GeoDataLink &GeoDataNetworkLink::link()
{
    return d->m_link;
}

const GeoDataLink& GeoDataNetworkLink::link() const
{
    return d->m_link;
}

void GeoDataNetworkLink::setLink(const GeoDataLink &link)
{
    d->m_link = link;
}

}
