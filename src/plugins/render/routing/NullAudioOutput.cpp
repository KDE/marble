//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#include "AudioOutput.h"

#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "routing/instructions/RoutingInstruction.h"

namespace Marble
{

class AudioOutputPrivate
{
public:
  void audioOutputFinished();
  void playInstructions();
};

void AudioOutputPrivate::audioOutputFinished()
{
    // nothing to do
}

void AudioOutputPrivate::playInstructions()
{
    // nothing to do
}

AudioOutput::AudioOutput( QObject* parent ) : QObject( parent ),
    d( new AudioOutputPrivate )
{
    // nothing to do
}

AudioOutput::~AudioOutput()
{
    delete d;
}

void AudioOutput::update( const Route &, qreal, qreal, bool )
{
    // nothing to do
}

void AudioOutput::setMuted( bool )
{
    // nothing to do
}

bool AudioOutput::isMuted() const
{
    return true;
}

void AudioOutput::setSpeaker( const QString & )
{
    // nothing to do
}

QString AudioOutput::speaker() const
{
    return QString();
}

void AudioOutput::setSoundEnabled( bool )
{
    // nothing to do
}

bool AudioOutput::isSoundEnabled() const
{
    return false;
}

}

#include "moc_AudioOutput.cpp"
