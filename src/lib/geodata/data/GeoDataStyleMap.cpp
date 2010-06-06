//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataStyleMap.h"
#include <QtCore/QDataStream>

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataStyleMapPrivate
{
  public:
    QString nodeType() const
    {
        return GeoDataTypes::GeoDataStyleMapType;
    }

    QString lastKey;
};


GeoDataStyleMap::GeoDataStyleMap()
    : d( new GeoDataStyleMapPrivate )
{
}

GeoDataStyleMap::GeoDataStyleMap( const GeoDataStyleMap& other )
    : GeoDataStyleSelector( other ) , QMap<QString,QString>(other), d( new GeoDataStyleMapPrivate( *other.d ) )

{
}

GeoDataStyleMap::~GeoDataStyleMap()
{
    delete d;
}

QString GeoDataStyleMap::nodeType() const
{
    return d->nodeType();
}

QString GeoDataStyleMap::lastKey() const
{
    return d->lastKey;
}

void GeoDataStyleMap::setLastKey( QString key )
{
    d->lastKey = key;
}

GeoDataStyleMap& GeoDataStyleMap::operator=( const GeoDataStyleMap& other )
{
    QMap<QString, QString>::operator=( other );
    GeoDataStyleSelector::operator=( other );
    *d = *other.d;
    return *this;
}

void GeoDataStyleMap::pack( QDataStream& stream ) const
{
    GeoDataStyleSelector::pack( stream );
    // lastKey doesn't need to be stored as it is needed at runtime only
    stream << *this;
}

void GeoDataStyleMap::unpack( QDataStream& stream )
{
    GeoDataStyleSelector::unpack( stream );
    
    stream >> *this;
}

}
