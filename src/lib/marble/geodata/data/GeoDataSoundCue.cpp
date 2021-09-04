// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "GeoDataSoundCue.h"
#include "GeoDataTypes.h"

namespace Marble
{

GeoDataSoundCue::GeoDataSoundCue() :
    m_delayedStart(0)
{
}

bool GeoDataSoundCue::operator==(const GeoDataSoundCue& other) const
{
    return this->m_delayedStart == other.m_delayedStart && this->m_href == other.m_href;
}

bool GeoDataSoundCue::operator!=(const GeoDataSoundCue& other) const
{
    return !this->operator==(other);
}

GeoDataSoundCue::~GeoDataSoundCue()
{
}

const char *GeoDataSoundCue::nodeType() const
{
    return GeoDataTypes::GeoDataSoundCueType;
}

QString GeoDataSoundCue::href() const
{
    return m_href;
}

void GeoDataSoundCue::setHref(const QString &url)
{
    m_href = url;
}

double GeoDataSoundCue::delayedStart() const
{
    return m_delayedStart;
}

void GeoDataSoundCue::setDelayedStart(double pause)
{
    m_delayedStart = pause;
}

} // namespace Marble
