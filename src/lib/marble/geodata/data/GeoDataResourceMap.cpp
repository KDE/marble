// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "GeoDataResourceMap.h"

#include "GeoDataAlias.h"
#include "GeoDataTypes.h"

namespace Marble {

class GeoDataResourceMapPrivate
{
public:
    GeoDataAlias m_alias;
    GeoDataResourceMapPrivate();
};

GeoDataResourceMapPrivate::GeoDataResourceMapPrivate() :
    m_alias()
{
    // nothing to do
}

GeoDataResourceMap::GeoDataResourceMap() : d( new GeoDataResourceMapPrivate )
{
    // nothing to do
}

GeoDataResourceMap::GeoDataResourceMap( const Marble::GeoDataResourceMap &other ) :
    GeoNode( other ), d( new GeoDataResourceMapPrivate( *other.d ) )
{
    // nothing to do
}

GeoDataResourceMap &GeoDataResourceMap::operator=( const GeoDataResourceMap &other )
{
    *d = *other.d;
    return *this;
}

bool GeoDataResourceMap::operator==( const GeoDataResourceMap &other ) const
{
    return d->m_alias == other.d->m_alias;
}

bool GeoDataResourceMap::operator!=( const GeoDataResourceMap &other ) const
{
    return !this->operator==( other );
}

GeoDataResourceMap::~GeoDataResourceMap()
{
    delete d;
}

const char *GeoDataResourceMap::nodeType() const
{
    return GeoDataTypes::GeoDataResourceMapType;
}

const GeoDataAlias &GeoDataResourceMap::alias() const
{
    return d->m_alias;
}

GeoDataAlias &GeoDataResourceMap::alias()
{
    return d->m_alias;
}

void GeoDataResourceMap::setAlias( const GeoDataAlias &alias )
{
    d->m_alias = alias;
}

QString GeoDataResourceMap::sourceHref() const
{
    return d->m_alias.sourceHref();
}

QString GeoDataResourceMap::targetHref() const
{
    return d->m_alias.targetHref();
}

void GeoDataResourceMap::setSourceHref( const QString& sourceHref )
{
    d->m_alias.setSourceHref( sourceHref );
}

void GeoDataResourceMap::setTargetHref( const QString& targetHref )
{
    d->m_alias.setTargetHref( targetHref );
}



}
