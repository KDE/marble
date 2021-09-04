// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "GeoDataTourControl.h"

#include "GeoDataTypes.h"

namespace Marble
{

GeoDataTourControl::GeoDataTourControl() :
    m_playMode(Play)
{
}

GeoDataTourControl::~GeoDataTourControl()
{
}

bool GeoDataTourControl::operator==(const GeoDataTourControl& other) const
{
    return this->m_playMode == other.m_playMode;
}

bool GeoDataTourControl::operator!=(const GeoDataTourControl& other) const
{
    return !this->operator==(other);
}

const char *GeoDataTourControl::nodeType() const
{
    return GeoDataTypes::GeoDataTourControlType;
}

GeoDataTourControl::PlayMode GeoDataTourControl::playMode() const
{
    return m_playMode;
}

void GeoDataTourControl::setPlayMode(PlayMode mode)
{
    m_playMode = mode;
}

} // namespace Marble
