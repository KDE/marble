// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "PlaybackSoundCueItem.h"

#include "GeoDataSoundCue.h"

#if HAVE_PHONON
#include <phonon/AudioOutput>
#endif

#include <QUrl>

namespace Marble
{
PlaybackSoundCueItem::PlaybackSoundCueItem(const GeoDataSoundCue *soundCue)
    : m_soundCue(soundCue)
    , m_href(soundCue->href())
{
#if HAVE_PHONON
    Phonon::createPath(&m_mediaObject, new Phonon::AudioOutput(Phonon::MusicCategory, this));
    m_mediaObject.setCurrentSource(QUrl(m_href));
#endif
}

const GeoDataSoundCue *PlaybackSoundCueItem::soundCue() const
{
    return m_soundCue;
}

double PlaybackSoundCueItem::duration() const
{
#if HAVE_PHONON
    return m_mediaObject.totalTime() * 1.0 / 1000;
#else
    return 0;
#endif
}

void PlaybackSoundCueItem::play()
{
#if HAVE_PHONON
    if (m_href != m_soundCue->href()) {
        m_mediaObject.setCurrentSource(QUrl(soundCue()->href()));
    }
    if (m_mediaObject.isValid()) {
        m_mediaObject.play();
    }
#endif
}

void PlaybackSoundCueItem::pause()
{
#if HAVE_PHONON
    m_mediaObject.pause();
#endif
}

void PlaybackSoundCueItem::seek(double progress)
{
#if HAVE_PHONON
    m_mediaObject.seek(progress * 1000);
#else
    Q_UNUSED(progress)
#endif
}

void PlaybackSoundCueItem::stop()
{
#if HAVE_PHONON
    m_mediaObject.stop();
#endif
}

}

#include "moc_PlaybackSoundCueItem.cpp"
