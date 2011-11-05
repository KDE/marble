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

#include "GeoDataLatLonAltBox.h"
#include "GeoDataTypes.h"
#include "MarbleDebug.h"

#include "GeoDataLineString.h"

#include <QtCore/QMap>
#include <QtCore/QLinkedList>
#include "GeoDataExtendedData.h"

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

GeoDataSimpleArrayData::GeoDataSimpleArrayData( const GeoDataGeometry &other )
    : GeoDataGeometry( other )
{
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

EnumGeometryId GeoDataSimpleArrayData::geometryId() const
{
    return GeoDataSimpleArrayDataId;
}

GeoDataLatLonAltBox GeoDataSimpleArrayData::latLonAltBox() const
{
    return GeoDataLatLonAltBox();
}

void GeoDataSimpleArrayData::pack( QDataStream& stream ) const
{
    GeoDataGeometry::pack( stream );
}

void GeoDataSimpleArrayData::unpack( QDataStream& stream )
{
    GeoDataGeometry::unpack( stream );
}


}