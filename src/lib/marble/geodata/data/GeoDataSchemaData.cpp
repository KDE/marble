//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#include "GeoDataSchemaData.h"

// Qt
#include <QDataStream>
#include <QString>
#include <QHash>

// Marble
#include "GeoDataSimpleData.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataSchemaDataPrivate
{
public:
    GeoDataSchemaDataPrivate()
    : m_parent( 0 )
    {
        // nothing to do
    }

    QString m_schemaUrl;
    QHash<QString, GeoDataSimpleData> m_simpleData;   // Map for name and SimpleData
    GeoNode *m_parent;
};

GeoDataSchemaData::GeoDataSchemaData()
    : d( new GeoDataSchemaDataPrivate )
{
}

GeoDataSchemaData::GeoDataSchemaData( const GeoDataSchemaData &other )
    : d( new GeoDataSchemaDataPrivate( *other.d ) )
{
}

GeoDataSchemaData &GeoDataSchemaData::operator=( const GeoDataSchemaData &rhs )
{
    *d = *rhs.d;
    return *this;
}

bool GeoDataSchemaData::operator==( const GeoDataSchemaData &other ) const
{
    if ( d->m_schemaUrl != other.d->m_schemaUrl ||
         d->m_simpleData.size() != other.d->m_simpleData.size() )
    {
        return false;
    }

    QHash<QString, GeoDataSimpleData>::iterator begin = d->m_simpleData.begin();
    QHash<QString, GeoDataSimpleData>::iterator end = d->m_simpleData.end();
    QHash<QString, GeoDataSimpleData>::iterator beginOther = other.d->m_simpleData.begin();

    for( ; begin != end; ++begin, ++beginOther ) {
        if ( *begin != *beginOther ) {
            return false;
        }
    }

    return true;
}

bool GeoDataSchemaData::operator!=( const GeoDataSchemaData &other ) const
{
    return !this->operator==(other);
}

GeoDataSchemaData::~GeoDataSchemaData()
{
    delete d;
}

QString GeoDataSchemaData::schemaUrl() const
{
    return d->m_schemaUrl;
}

void GeoDataSchemaData::setSchemaUrl( const QString &schemaUrl )
{
    d->m_schemaUrl = schemaUrl;
}

GeoDataSimpleData &GeoDataSchemaData::simpleData( const QString &name ) const
{
    return d->m_simpleData[ name ];
}

void GeoDataSchemaData::addSimpleData( const GeoDataSimpleData &data )
{
    d->m_simpleData.insert( data.name(), data );
}

QList<GeoDataSimpleData> GeoDataSchemaData::simpleDataList() const
{
    return d->m_simpleData.values();
}

void GeoDataSchemaData::setParent( GeoNode *parent )
{
    d->m_parent = parent;
}

const GeoNode *GeoDataSchemaData::parent() const
{
    return d->m_parent;
}

GeoNode *GeoDataSchemaData::parent()
{
    return d->m_parent;
}

const char* GeoDataSchemaData::nodeType() const
{
    return GeoDataTypes::GeoDataSchemaDataType;
}

void GeoDataSchemaData::pack( QDataStream &stream ) const
{
    stream << d->m_schemaUrl;
    stream << d->m_simpleData.size();

    QHash<QString, GeoDataSimpleData>::const_iterator iter = d->m_simpleData.constBegin();
    QHash<QString, GeoDataSimpleData>::const_iterator end = d->m_simpleData.constEnd();

    for( ; iter != end; ++iter ) {
        iter.value().pack( stream );
    }
}

void GeoDataSchemaData::unpack( QDataStream &stream )
{
    stream >> d->m_schemaUrl;
    int size = 0;
    stream >> size;

    for( int i = 0; i < size; i++ ) {
        GeoDataSimpleData simpleData;
        simpleData.unpack( stream );
        d->m_simpleData.insert( simpleData.name(), simpleData );
    }
}

}   // namespace Marble
