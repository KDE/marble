//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//

#include "GeoDataSimpleArrayData.h"

#include "GeoDataTypes.h"
#include "MarbleDebug.h"

#include <QtCore/QMap>
#include <QtCore/QLinkedList>

namespace Marble {

class GeoDataSimpleArrayDataPrivate
{
public:
    GeoDataSimpleArrayDataPrivate()
    {
    }

    QList< QVariant > m_values;
};

GeoDataSimpleArrayData::GeoDataSimpleArrayData()
    : d( new GeoDataSimpleArrayDataPrivate() )
{
}

GeoDataSimpleArrayData::GeoDataSimpleArrayData( const GeoDataSimpleArrayData& other )
    : GeoDataObject( other ), d( new GeoDataSimpleArrayDataPrivate( *other.d ) )
{
}

GeoDataSimpleArrayData::~GeoDataSimpleArrayData()
{
    delete d;
}

int GeoDataSimpleArrayData::size() const
{
    return d->m_values.size();
}

QVariant GeoDataSimpleArrayData::valueAt(int index) const
{
    return d->m_values.at( index );
}

QList< QVariant > GeoDataSimpleArrayData::valuesList() const
{
    return d->m_values;
}

void GeoDataSimpleArrayData::append( const QVariant& value )
{
    d->m_values.append( value );
}


const char* GeoDataSimpleArrayData::nodeType() const
{
    return GeoDataTypes::GeoDataSimpleArrayDataType;
}

void GeoDataSimpleArrayData::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );
}

void GeoDataSimpleArrayData::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );
}


}