//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "PlaybackTourControlItem.h"

#include "GeoDataTourControl.h"

namespace Marble
{

PlaybackTourControlItem::PlaybackTourControlItem(const GeoDataTourControl* tourControl)
{
    m_tourControl = tourControl;
}

const GeoDataTourControl* PlaybackTourControlItem::tourControl() const
{
    return m_tourControl;
}

void PlaybackTourControlItem::play()
{
    if( m_tourControl->playMode() == GeoDataTourControl::Pause ) {
        emit paused();
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

void PlaybackTourControlItem::seek( double position )
{
    Q_UNUSED( position );
    // nothing to do
}

void PlaybackTourControlItem::stop()
{
    // nothing to do
}

}
