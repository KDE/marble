//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

// Own
#include "GeoDataTimeStamp.h"

// Private
#include "GeoDataTimeStamp_p.h"

// Qt
#include <QtCore/QDataStream>

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

const char* GeoDataTimeStamp::nodeType() const
{
    return d->nodeType();
}

QDateTime GeoDataTimeStamp::when() const
{
    return d->m_when;
}

void GeoDataTimeStamp::setWhen( const QDateTime& when )
{
    d->m_when = when;
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
