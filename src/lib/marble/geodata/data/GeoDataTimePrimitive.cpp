// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

// Own
#include "GeoDataTimePrimitive.h"

// Private
#include "GeoDataTimePrimitive_p.h"

// Qt
#include <QDataStream>

// GeoData
#include "GeoDataTypes.h"

namespace Marble
{

GeoDataTimePrimitive::GeoDataTimePrimitive()
    : GeoDataObject()
    , d(new GeoDataTimePrimitivePrivate)
{
}

GeoDataTimePrimitive::GeoDataTimePrimitive(const GeoDataTimePrimitive &other)
    : GeoDataObject(other)
    , d(new GeoDataTimePrimitivePrivate(*other.d))
{
}

GeoDataTimePrimitive::~GeoDataTimePrimitive()
{
    delete d;
}

GeoDataTimePrimitive &GeoDataTimePrimitive::operator=(const GeoDataTimePrimitive &other)
{
    GeoDataObject::operator=(other);
    *d = *other.d;
    return *this;
}

const char *GeoDataTimePrimitive::nodeType() const
{
    return GeoDataTypes::GeoDataTimePrimitiveType;
}

void GeoDataTimePrimitive::pack(QDataStream &stream) const
{
    GeoDataObject::pack(stream);
}

void GeoDataTimePrimitive::unpack(QDataStream &stream)
{
    GeoDataObject::unpack(stream);
}

}
