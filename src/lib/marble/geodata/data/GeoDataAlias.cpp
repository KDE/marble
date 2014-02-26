//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "GeoDataAlias.h"
#include "GeoDataTypes.h"

namespace Marble {

class GeoDataAliasPrivate
{
public:
    QString m_sourceHref;
    QString m_targetHref;

    GeoDataAliasPrivate();
};

GeoDataAliasPrivate::GeoDataAliasPrivate() :
    m_sourceHref(), m_targetHref()
{
    // nothing to do
}

GeoDataAlias::GeoDataAlias() : d( new GeoDataAliasPrivate )
{
    // nothing to do
}

GeoDataAlias::GeoDataAlias( const Marble::GeoDataAlias &other ) :
   GeoNode( other ), d( new GeoDataAliasPrivate( *other.d ) )
{
    // nothing to do
}

GeoDataAlias &GeoDataAlias::operator=( const GeoDataAlias &other )
{
    *d = *other.d;
    return *this;
}

bool GeoDataAlias::operator==( const GeoDataAlias &other ) const
{
    return ( d->m_sourceHref == other.d->m_sourceHref ) && ( d->m_targetHref == other.d->m_targetHref );
}

bool GeoDataAlias::operator!=( const GeoDataAlias &other ) const
{
    return !this->operator==(other);
}

GeoDataAlias::~GeoDataAlias()
{
    delete d;
}

const char *GeoDataAlias::nodeType() const
{
    return GeoDataTypes::GeoDataAliasType;
}

QString GeoDataAlias::sourceHref() const
{
    return d->m_sourceHref;
}

QString GeoDataAlias::targetHref() const
{
    return d->m_targetHref;
}

void GeoDataAlias::setSourceHref( const QString& sourceHref )
{
    d->m_sourceHref = sourceHref;
}

void GeoDataAlias::setTargetHref( const QString& targetHref )
{
    d->m_targetHref = targetHref;
}

}
