//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "GeoDataBuilding.h"
#include "GeoDataBuilding_p.h"
#include "GeoDataTypes.h"

namespace Marble {

GeoDataBuilding::GeoDataBuilding()
    : GeoDataGeometry(new GeoDataBuildingPrivate),
      d(new GeoDataBuildingPrivate)
{
}

GeoDataBuilding::GeoDataBuilding(const GeoDataGeometry &other)
    : GeoDataGeometry(other),
      d(new GeoDataBuildingPrivate)
{
}

GeoDataBuilding::GeoDataBuilding(const GeoDataBuilding &other)
    : GeoDataGeometry(other),
      d(new GeoDataBuildingPrivate(*other.d))
{
}

GeoDataBuilding::~GeoDataBuilding()
{
    delete d;
}

GeoDataBuilding& GeoDataBuilding::operator=(const GeoDataBuilding& other)
{
    GeoDataGeometry::operator=(other);
    *d = *other.d;
    return *this;
}

const char *GeoDataBuilding::nodeType() const
{
    return GeoDataTypes::GeoDataBuildingType;
}

EnumGeometryId GeoDataBuilding::geometryId() const
{
    return GeoDataBuildingId;
}

GeoDataGeometry *GeoDataBuilding::copy() const
{
    return new GeoDataBuilding(*this);
}

double GeoDataBuilding::height() const
{
    return d->m_height;
}

void GeoDataBuilding::setHeight(double height)
{
    d->m_height = height;
}

int GeoDataBuilding::minLevel() const
{
    return d->m_minLevel;
}

void GeoDataBuilding::setMinLevel(int minLevel)
{
    d->m_minLevel = minLevel;
}

int GeoDataBuilding::maxLevel() const
{
    return d->m_maxLevel;
}

void GeoDataBuilding::setMaxLevel(int maxLevel)
{
    d->m_maxLevel = maxLevel;
}

QVector<int> GeoDataBuilding::nonExistentLevels() const
{
    return d->m_nonExistentLevels;
}

void GeoDataBuilding::setNonExistentLevels(const QVector<int> &nonExistentLevels)
{
    d->m_nonExistentLevels = nonExistentLevels;
}

GeoDataMultiGeometry* GeoDataBuilding::multiGeometry() const
{
    return &d->m_multiGeometry;
}

}
