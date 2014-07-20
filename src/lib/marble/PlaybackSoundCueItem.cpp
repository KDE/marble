//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "PlaybackSoundCueItem.h"

#include "GeoDataSoundCue.h"

#ifdef HAVE_PHONON
#include <phonon/AudioOutput>
#endif

#include <QUrl>

namespace Marble
{
PlaybackSoundCueItem::PlaybackSoundCueItem( const GeoDataSoundCue* soundCue )
{
    m_soundCue = soundCue;
#ifdef HAVE_PHONON
    Phonon::createPath( &m_mediaObject, new Phonon::AudioOutput( Phonon::MusicCategory, this ) );
    m_mediaObject.setCurrentSource( QUrl( soundCue->href() ) );
#endif
}

const GeoDataSoundCue* PlaybackSoundCueItem::soundCue() const
{
    return m_soundCue;
}

double PlaybackSoundCueItem::duration() const
{
#ifdef HAVE_PHONON
    return m_mediaObject.totalTime() * 1.0 / 1000;
#else
    return 0;
#endif
}

void PlaybackSoundCueItem::play()
{
#ifdef HAVE_PHONON
    m_mediaObject.play();
#endif
}

void PlaybackSoundCueItem::pause()
{
#ifdef HAVE_PHONON
    m_mediaObject.pause();
#endif
}

void PlaybackSoundCueItem::seek( double progress )
{
#ifdef HAVE_PHONON
    m_mediaObject.seek( progress * 1000 );
#endif
}

void PlaybackSoundCueItem::stop()
{
#ifdef HAVE_PHONON
    m_mediaObject.stop();
#endif
}

}

#include "PlaybackSoundCueItem.moc"
