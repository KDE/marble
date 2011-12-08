//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
// Copyright 2011      Niko Sams <niko.sams@gmail.com>
//

#include "GeoDataExtendedData.h"
#include "GeoDataExtendedData_p.h"
#include <QtCore/QDataStream>

#include "GeoDataTypes.h"

namespace Marble
{

GeoDataExtendedData::GeoDataExtendedData()
    : GeoDataObject(), d( new GeoDataExtendedDataPrivate )
{
}

GeoDataExtendedData::GeoDataExtendedData( const GeoDataExtendedData& other )
    : GeoDataObject( other ), d( new GeoDataExtendedDataPrivate( *other.d ) )
{
}

GeoDataExtendedData::~GeoDataExtendedData()
{
    delete d;
}

GeoDataExtendedData& GeoDataExtendedData::operator=( const GeoDataExtendedData& other )
{
    GeoDataObject::operator=( other );
    *d = *other.d;
    return *this;
}

const char* GeoDataExtendedData::nodeType() const
{
    return d->nodeType();
}

GeoDataData& GeoDataExtendedData::valueRef( const QString& key ) const
{
    return d->hash[ key ];
}

GeoDataData GeoDataExtendedData::value( const QString& key ) const
{
    return d->hash.value( key );
}

void GeoDataExtendedData::addValue( const GeoDataData& data )
{
    d->hash.insert( data.name(), data );
}

QHash< QString, GeoDataData >::const_iterator GeoDataExtendedData::constBegin( ) const
{
	return d->hash.constBegin();
}

QHash< QString, GeoDataData >::const_iterator GeoDataExtendedData::constEnd( ) const
{
	return d->hash.constEnd();
}

int GeoDataExtendedData::size() const
{
    return d->hash.size();
}

bool GeoDataExtendedData::isEmpty( ) const
{
    return d->hash.empty();
}

bool GeoDataExtendedData::contains( const QString &key ) const
{
    return d->hash.contains( key );
}

void GeoDataExtendedData::setSimpleArrayData( const QString& key, GeoDataSimpleArrayData *values )
{
    d->arrayHash[ key ] = values;
}

GeoDataSimpleArrayData* GeoDataExtendedData::simpleArrayData( const QString& key ) const
{
    if ( !d->arrayHash.contains( key ) ) return 0;
    return d->arrayHash[ key ];
}

void GeoDataExtendedData::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );
}

void GeoDataExtendedData::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );
}

}
