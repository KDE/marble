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

namespace Marble
{
PlaybackSoundCueItem::PlaybackSoundCueItem( const GeoDataSoundCue* soundCue )
{
    m_soundCue = soundCue;
#ifdef HAVE_PHONON
    Phonon::MediaObject *mediaObject = new Phonon::MediaObject( );
    Phonon::createPath( mediaObject, new Phonon::AudioOutput( Phonon::MusicCategory ) );
    mediaObject->setCurrentSource( QUrl( soundCue->href() ) );
    m_mediaObject = mediaObject;
#endif
}

const GeoDataSoundCue* PlaybackSoundCueItem::soundCue() const
{
    return m_soundCue;
}

#ifdef HAVE_PHONON
Phonon::MediaObject* PlaybackSoundCueItem::mediaObject()
{
    return m_mediaObject;
}
#endif

double PlaybackSoundCueItem::duration() const
{
#ifdef HAVE_PHONON
    return m_mediaObject->totalTime() * 1.0 / 1000;
#else
    return 0;
#endif
}

void PlaybackSoundCueItem::play()
{
#ifdef HAVE_PHONON
    m_mediaObject->play();
#endif
}

void PlaybackSoundCueItem::pause()
{
#ifdef HAVE_PHONON
    m_mediaObject->pause();
#endif
}

void PlaybackSoundCueItem::seek( double progress )
{
#ifdef HAVE_PHONON
    m_mediaObject->seek( progress * 1000 );
#endif
}

void PlaybackSoundCueItem::stop()
{
#ifdef HAVE_PHONON
    m_mediaObject->stop();
#endif
}

}