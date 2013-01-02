//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "AudioOutput.h"

#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "routing/VoiceNavigationModel.h"

#include <QtCore/QDirIterator>
#include <phonon/MediaObject>
#include <phonon/MediaSource>
#include <phonon/AudioOutput>

namespace Marble
{

class AudioOutputPrivate
{
public:
    AudioOutput *q;

    Phonon::MediaObject *m_output;

    bool m_muted;

    VoiceNavigationModel m_voiceNavigation;

    AudioOutputPrivate( AudioOutput* parent );

    void audioOutputFinished();

    void setupAudio();

    void reset();

    void playInstructions();
};

AudioOutputPrivate::AudioOutputPrivate( AudioOutput* parent ) :
    q( parent ), m_output( 0 ), m_muted( false )
{
    QObject::connect( &m_voiceNavigation, SIGNAL(instructionChanged()),
                      q, SLOT(playInstructions()) );
}

void AudioOutputPrivate::audioOutputFinished()
{
    m_output->setCurrentSource( QString() );
    m_output->clearQueue();
}

void AudioOutputPrivate::setupAudio()
{
    if ( !m_output ) {
        m_output = new Phonon::MediaObject( q );
        Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput( Phonon::VideoCategory, q );
        Phonon::createPath( m_output, audioOutput );

        q->connect( m_output, SIGNAL(finished()), q, SLOT(audioOutputFinished()) );
    }
}

void AudioOutputPrivate::reset()
{
    if ( m_output ) {
        m_output->stop();
        m_output->setCurrentSource( QString() );
        m_output->clearQueue();
    }

    m_voiceNavigation.reset();
}

void AudioOutputPrivate::playInstructions()
{
    setupAudio();
    if ( m_output ) {
        m_output->enqueue( m_voiceNavigation.instruction() );
        m_output->play();
    }
}

AudioOutput::AudioOutput( QObject* parent ) : QObject( parent ),
    d( new AudioOutputPrivate( this ) )
{
    setSoundEnabled( false );
}

AudioOutput::~AudioOutput()
{
    delete d;
}

void AudioOutput::update(const Route &route, qreal distanceManeuver, qreal distanceTarget, bool deviated )
{
    d->m_voiceNavigation.update( route, distanceManeuver, distanceTarget, deviated );
}

void AudioOutput::setMuted( bool muted )
{
    d->m_muted = muted;
}

void AudioOutput::setSpeaker( const QString &speaker )
{
    d->m_voiceNavigation.setSpeaker( speaker );
}

QString AudioOutput::speaker() const
{
    return d->m_voiceNavigation.speaker();
}

void AudioOutput::setSoundEnabled( bool enabled )
{
    d->m_voiceNavigation.setSpeakerEnabled( !enabled );
}

}

#include "AudioOutput.moc"
