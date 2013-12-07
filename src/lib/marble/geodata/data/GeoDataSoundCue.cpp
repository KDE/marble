//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "GeoDataSoundCue.h"
#include "GeoDataTypes.h"

namespace Marble
{

GeoDataSoundCue::GeoDataSoundCue() :
    m_delayedStart(0)
{
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

QString GeoDataSoundCue::id() const
{
    return m_id;
}

void GeoDataSoundCue::setId(const QString &i)
{
    m_id = i;
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
