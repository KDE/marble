//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mihail Ivchenko <ematirov@gmail.com>
//

#include "GeoDataWait.h"

#include "GeoDataTypes.h"

namespace Marble
{

GeoDataWait::GeoDataWait() :
    m_duration(0.0)
{
}

GeoDataWait::~GeoDataWait()
{
}

bool GeoDataWait::operator==(const GeoDataWait& other) const
{
    return this->m_duration == other.m_duration;
}

bool GeoDataWait::operator!=(const GeoDataWait& other) const
{
    return !this->operator==(other);
}

const char *GeoDataWait::nodeType() const
{
    return GeoDataTypes::GeoDataWaitType;
}

double GeoDataWait::duration() const
{
    return m_duration;
}

void GeoDataWait::setDuration(double duration)
{
    m_duration = duration;
}

} // namespace Marble
