// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Niko Sams <niko.sams@gmail.com>
//

#include "GeoDataSimpleArrayData.h"

#include "GeoDataTypes.h"
#include "MarbleDebug.h"

#include <QVariant>

namespace Marble
{

class GeoDataSimpleArrayDataPrivate
{
public:
    GeoDataSimpleArrayDataPrivate() = default;

    QList<QVariant> m_values;
};

GeoDataSimpleArrayData::GeoDataSimpleArrayData()
    : d(new GeoDataSimpleArrayDataPrivate())
{
}

GeoDataSimpleArrayData::GeoDataSimpleArrayData(const GeoDataSimpleArrayData &other)
    : GeoDataObject(other)
    , d(new GeoDataSimpleArrayDataPrivate(*other.d))
{
}

bool GeoDataSimpleArrayData::operator==(const GeoDataSimpleArrayData &other) const
{
    return equals(other) && d->m_values == other.d->m_values;
}

bool GeoDataSimpleArrayData::operator!=(const GeoDataSimpleArrayData &other) const
{
    return !this->operator==(other);
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
    return d->m_values.at(index);
}

QList<QVariant> GeoDataSimpleArrayData::valuesList() const
{
    return d->m_values;
}

void GeoDataSimpleArrayData::append(const QVariant &value)
{
    d->m_values.append(value);
}

const char *GeoDataSimpleArrayData::nodeType() const
{
    return GeoDataTypes::GeoDataSimpleArrayDataType;
}

void GeoDataSimpleArrayData::pack(QDataStream &stream) const
{
    GeoDataObject::pack(stream);
}

void GeoDataSimpleArrayData::unpack(QDataStream &stream)
{
    GeoDataObject::unpack(stream);
}

}
