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
    m_id(""),
    m_duration(0.0)
{
}

GeoDataWait::~GeoDataWait()
{
}

const char *GeoDataWait::nodeType() const
{
    return GeoDataTypes::GeoDataWaitType;
}

QString GeoDataWait::id() const
{
    return m_id;
}

void GeoDataWait::setId(const QString &id)
{
    m_id = id;
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
