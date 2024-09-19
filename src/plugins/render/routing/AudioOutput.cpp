// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#include "AudioOutput.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "routing/Route.h"
#include "routing/VoiceNavigationModel.h"

#include <QUrl>
#include <phonon/AudioOutput>
#include <phonon/MediaObject>
#include <phonon/MediaSource>

namespace Marble
{

class AudioOutputPrivate
{
public:
    AudioOutput *q;

    Phonon::MediaObject *m_output;

    bool m_muted;

    VoiceNavigationModel m_voiceNavigation;

    AudioOutputPrivate(AudioOutput *parent);

    void audioOutputFinished();

    void setupAudio();

    void reset();

    void playInstructions();
};

AudioOutputPrivate::AudioOutputPrivate(AudioOutput *parent)
    : q(parent)
    , m_output(nullptr)
    , m_muted(false)
{
    QObject::connect(&m_voiceNavigation, SIGNAL(instructionChanged()), q, SLOT(playInstructions()));
}

void AudioOutputPrivate::audioOutputFinished()
{
    m_output->setCurrentSource(Phonon::MediaSource());
    m_output->clearQueue();
}

void AudioOutputPrivate::setupAudio()
{
    if (!m_output) {
        m_output = new Phonon::MediaObject(q);
        Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, q);
        Phonon::createPath(m_output, audioOutput);

        q->connect(m_output, SIGNAL(finished()), q, SLOT(audioOutputFinished()));
    }
}

void AudioOutputPrivate::reset()
{
    if (m_output) {
        m_output->stop();
        m_output->setCurrentSource(Phonon::MediaSource());
        m_output->clearQueue();
    }

    m_voiceNavigation.reset();
}

void AudioOutputPrivate::playInstructions()
{
    setupAudio();
    if (m_output) {
        m_output->enqueue(QUrl::fromLocalFile(m_voiceNavigation.instruction()));
        m_output->play();
    }
}

AudioOutput::AudioOutput(QObject *parent)
    : QObject(parent)
    , d(new AudioOutputPrivate(this))
{
    setSoundEnabled(false);
}

AudioOutput::~AudioOutput()
{
    delete d;
}

void AudioOutput::update(const Route &route, qreal distanceManeuver, qreal distanceTarget, bool deviated)
{
    d->m_voiceNavigation.update(route, distanceManeuver, distanceTarget, deviated);
}

void AudioOutput::setMuted(bool muted)
{
    d->m_muted = muted;
}

bool AudioOutput::isMuted() const
{
    return d->m_muted;
}

void AudioOutput::setSpeaker(const QString &speaker)
{
    d->m_voiceNavigation.setSpeaker(speaker);
}

QString AudioOutput::speaker() const
{
    return d->m_voiceNavigation.speaker();
}

void AudioOutput::setSoundEnabled(bool enabled)
{
    d->m_voiceNavigation.setSpeakerEnabled(!enabled);
}

bool AudioOutput::isSoundEnabled() const
{
    return !d->m_voiceNavigation.isSpeakerEnabled();
}

}

#include "moc_AudioOutput.cpp"
