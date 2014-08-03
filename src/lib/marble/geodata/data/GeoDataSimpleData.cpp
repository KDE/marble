//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

// Qt
#include <QString>
#include <QDataStream>

// Marble
#include "GeoDataSimpleData.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataSimpleDataPrivate
{
public:
    QString m_name;
    QString m_data;
};

GeoDataSimpleData::GeoDataSimpleData()
    : d( new GeoDataSimpleDataPrivate )
{
}

GeoDataSimpleData::GeoDataSimpleData( const GeoDataSimpleData &other )
    : d( new GeoDataSimpleDataPrivate( *other.d ) )
{
}

GeoDataSimpleData::~GeoDataSimpleData()
{
    delete d;
}

QString GeoDataSimpleData::name() const
{
    return d->m_name;
}

void GeoDataSimpleData::setName( const QString &name )
{
    d->m_name = name;
}

QString GeoDataSimpleData::data() const
{
    return d->m_data;
}

void GeoDataSimpleData::setData( const QString &data )
{
    d->m_data = data;
}

GeoDataSimpleData &GeoDataSimpleData::operator=( const GeoDataSimpleData &rhs )
{
    *d = *rhs.d;
    return *this;
}

bool GeoDataSimpleData::operator==( const GeoDataSimpleData &other ) const
{
    return d->m_name == other.d->m_name &&
           d->m_data == other.d->m_data;
}

bool GeoDataSimpleData::operator!=( const GeoDataSimpleData &other ) const
{
    return !this->operator==( other );
}

const char* GeoDataSimpleData::nodeType() const
{
    return GeoDataTypes::GeoDataSimpleDataType;
}

void GeoDataSimpleData::pack( QDataStream &stream ) const
{
    stream << d->m_name;
    stream << d->m_data;
}

void GeoDataSimpleData::unpack( QDataStream &stream )
{
    stream >> d->m_name;
    stream >> d->m_data;
}

}
