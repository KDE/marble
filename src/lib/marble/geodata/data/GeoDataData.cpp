//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#include "GeoDataData.h"
#include "GeoDataData_p.h"

#include <QDataStream>

#include "GeoDataTypes.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble
{

GeoDataData::GeoDataData()
    : GeoDataObject(), d( new GeoDataDataPrivate )
{
}

GeoDataData::GeoDataData( const GeoDataData& other )
    : GeoDataObject( other ), d( new GeoDataDataPrivate( *other.d ) )
{
}

GeoDataData::~GeoDataData()
{
    delete d;
}

GeoDataData& GeoDataData::operator=( const GeoDataData& other )
{
    GeoDataObject::operator=( other );
    *d = *other.d;
    return *this;
}

bool GeoDataData::operator==( const GeoDataData& other) const
{
    return equals(other) &&
           d->m_name == other.d->m_name &&
           d->m_value == other.d->m_value &&
           d->m_displayName == other.d->m_displayName;
}

bool GeoDataData::operator!=( const GeoDataData &other ) const
{
    return !this->operator==(other);
}

GeoDataData::GeoDataData( const QString &name, const QVariant &value )
    : d( new GeoDataDataPrivate )
{
    d->m_name = name;
    d->m_value = value;
}

const char* GeoDataData::nodeType() const
{
    return d->nodeType();
}

QVariant GeoDataData::value() const
{
    return d->m_value;
}

QVariant& GeoDataData::valueRef()
{
    return d->m_value;
}

const QVariant& GeoDataData::valueRef() const
{
    return d->m_value;
}

void GeoDataData::setValue( const QVariant& value )
{
    d->m_value = value;
}

QString GeoDataData::name() const
{
    return d->m_name;
}

void GeoDataData::setName( const QString& name )
{
    d->m_name = name;
}

QString GeoDataData::displayName() const
{
    return d->m_displayName;
}

void GeoDataData::setDisplayName( const QString& displayName )
{
    d->m_displayName = displayName;
}

void GeoDataData::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << d->m_value;
    stream << d->m_displayName;
}

void GeoDataData::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    stream >> d->m_value;
    stream >> d->m_displayName;
}

}
