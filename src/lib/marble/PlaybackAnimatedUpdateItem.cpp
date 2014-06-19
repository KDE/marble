//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "PlaybackAnimatedUpdateItem.h"

namespace Marble
{
PlaybackAnimatedUpdateItem::PlaybackAnimatedUpdateItem( const GeoDataAnimatedUpdate* animatedUpdate )
{
    m_animatedUpdate = animatedUpdate;
}

const GeoDataAnimatedUpdate* PlaybackAnimatedUpdateItem::animatedUpdate() const
{
    return m_animatedUpdate;
}

double PlaybackAnimatedUpdateItem::duration() const
{
    return m_animatedUpdate->duration();
}

void PlaybackAnimatedUpdateItem::play()
{
    /** @todo Implement */
}

void PlaybackAnimatedUpdateItem::pause()
{
    /** @todo Implement */
}

void PlaybackAnimatedUpdateItem::seek( double position )
{
    Q_UNUSED( position );
    /** @todo Implement */
}

void PlaybackAnimatedUpdateItem::stop()
{
    /** @todo Implement */
}

}