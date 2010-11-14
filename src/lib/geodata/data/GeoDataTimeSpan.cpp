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
#include "GeoDataTimeSpan.h"

// Private
#include "GeoDataTimeSpan_p.h"

// Qt
#include <QtCore/QDataStream>

// GeoData
#include "GeoDataTypes.h"
#include "MarbleDebug.h"

namespace Marble
{

GeoDataTimeSpan::GeoDataTimeSpan()
    : GeoDataTimePrimitive(), d( new GeoDataTimeSpanPrivate )
{
}

GeoDataTimeSpan::GeoDataTimeSpan( const GeoDataTimeSpan& other )
    : GeoDataTimePrimitive( other ), d( new GeoDataTimeSpanPrivate( *other.d ) )
{
}

GeoDataTimeSpan::~GeoDataTimeSpan()
{
    delete d;
}

QString GeoDataTimeSpan::nodeType() const
{
    return d->nodeType();
}

QDateTime GeoDataTimeSpan::end() const
{
    return d->m_end;
}

void GeoDataTimeSpan::setEnd( const QDateTime& end )
{
    d->m_end = end;
}

QDateTime GeoDataTimeSpan::begin() const
{
    return d->m_begin;
}

void GeoDataTimeSpan::setBegin( const QDateTime& begin )
{
    d->m_begin = begin;
}

GeoDataTimeSpan& GeoDataTimeSpan::operator=( const GeoDataTimeSpan& other )
{
    GeoDataTimePrimitive::operator=( other );
    *d = *other.d;
    return *this;
}

void GeoDataTimeSpan::pack( QDataStream& stream ) const
{
    GeoDataTimePrimitive::pack( stream );

    stream << d->m_begin;
    stream << d->m_end;
}

void GeoDataTimeSpan::unpack( QDataStream& stream )
{
    GeoDataTimePrimitive::unpack( stream );

    stream >> d->m_begin;
    stream >> d->m_end;    
}

}
