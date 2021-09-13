// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

// Own
#include "GeoDataTimeStamp.h"

// Private
#include "GeoDataTimeStamp_p.h"

// Qt
#include <QDataStream>

// GeoData
#include "GeoDataTypes.h"

namespace Marble
{

GeoDataTimeStamp::GeoDataTimeStamp()
    : GeoDataTimePrimitive(), d( new GeoDataTimeStampPrivate )
{
}

GeoDataTimeStamp::GeoDataTimeStamp( const GeoDataTimeStamp& other )
    : GeoDataTimePrimitive( other ) , d( new GeoDataTimeStampPrivate( *other.d ) )
{
}

GeoDataTimeStamp::~GeoDataTimeStamp()
{
    delete d;
}

GeoDataTimeStamp& GeoDataTimeStamp::operator=( const GeoDataTimeStamp& other )
{
    GeoDataTimePrimitive::operator=( other );
    *d = *other.d;
    return *this;
}

bool GeoDataTimeStamp::operator==( const GeoDataTimeStamp& other ) const
{
    return equals(other) &&
           d->m_resolution == other.d->m_resolution &&
           d->m_when == other.d->m_when;
}

bool GeoDataTimeStamp::operator!=( const GeoDataTimeStamp& other ) const
{
    return !this->operator==( other );
}


const char* GeoDataTimeStamp::nodeType() const
{
    return GeoDataTypes::GeoDataTimeStampType;
}

QDateTime GeoDataTimeStamp::when() const
{
    return d->m_when;
}

void GeoDataTimeStamp::setWhen( const QDateTime& when )
{
    d->m_when = when;
}

void GeoDataTimeStamp::setResolution( GeoDataTimeStamp::TimeResolution resolution )
{
    d->m_resolution = resolution;
}

GeoDataTimeStamp::TimeResolution GeoDataTimeStamp::resolution() const
{
    return d->m_resolution;
}

void GeoDataTimeStamp::pack( QDataStream& stream ) const
{
    GeoDataTimePrimitive::pack( stream );

    stream << d->m_when;
}

void GeoDataTimeStamp::unpack( QDataStream& stream )
{
    GeoDataTimePrimitive::unpack( stream );

    stream >> d->m_when;
}

}
