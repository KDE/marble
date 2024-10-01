// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "PlaybackTourControlItem.h"

#include "GeoDataTourControl.h"

namespace Marble
{

PlaybackTourControlItem::PlaybackTourControlItem(const GeoDataTourControl *tourControl)
{
    m_tourControl = tourControl;
}

const GeoDataTourControl *PlaybackTourControlItem::tourControl() const
{
    return m_tourControl;
}

void PlaybackTourControlItem::play()
{
    if (m_tourControl->playMode() == GeoDataTourControl::Pause) {
        Q_EMIT paused();
    }
}

double PlaybackTourControlItem::duration() const
{
    return 0;
}

void PlaybackTourControlItem::pause()
{
    // nothing to do
}

void PlaybackTourControlItem::seek(double position)
{
    Q_UNUSED(position);
    // nothing to do
}

void PlaybackTourControlItem::stop()
{
    // nothing to do
}

}
