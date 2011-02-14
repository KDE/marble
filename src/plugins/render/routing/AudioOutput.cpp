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
#include "routing/instructions/RoutingInstruction.h"

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

    QMap<RoutingInstruction::TurnType, QString> m_turnTypeMap;

    QString m_speaker;

    Phonon::MediaObject *m_output;

    int m_lastIndex;

    qreal m_lastDistance;

    RoutingInstruction::TurnType m_lastTurnType;

    bool m_muted;

    bool m_soundEnabled;

    AudioOutputPrivate( AudioOutput* parent );

    void audioOutputFinished();

    void setupAudio( qreal distance, RoutingInstruction::TurnType turnType );

    QString distanceAudioFile( qreal distance );

    QString turnTypeAudioFile( RoutingInstruction::TurnType turnType );

    QString audioFile( const QString &name );

    void reset();
};

AudioOutputPrivate::AudioOutputPrivate( AudioOutput* parent ) :
    q( parent ), m_output( 0 ), m_lastIndex( -1 ),
    m_lastDistance( 0.0 ), m_muted( false ), m_soundEnabled( false )
{
    // nothing to do
}

void AudioOutputPrivate::audioOutputFinished()
{
    m_output->clearQueue();
}

QString AudioOutputPrivate::distanceAudioFile( qreal dest )
{
    if ( dest > 0.0 && dest < 900.0 ) {
        qreal minDistance = 0.0;
        int targetDistance = 0;
        QVector<int> distances;
        distances << 50 << 80 << 100 << 200 << 300 << 400 << 500 << 600 << 700 << 800;
        foreach( int distance, distances ) {
            QString file = audioFile( QString::number( distance ) );
            qreal currentDistance = qAbs( distance - dest );
            if ( !file.isEmpty() && ( minDistance == 0.0 || currentDistance < minDistance ) ) {
                minDistance = currentDistance;
                targetDistance = distance;
            }
        }

        if ( targetDistance > 0 ) {
            return audioFile( QString::number( targetDistance ) );
        }
    }

    return QString();
}

QString AudioOutputPrivate::turnTypeAudioFile( RoutingInstruction::TurnType turnType )
{
    if ( m_turnTypeMap.contains( turnType ) ) {
        return audioFile( m_turnTypeMap[turnType] );
    }

    return QString();
}

QString AudioOutputPrivate::audioFile( const QString &name )
{
    if ( m_soundEnabled ) {
        QString const audioTemplate = "audio/%1.ogg";
        return MarbleDirs::path( audioTemplate.arg( name ) );
    } else {
        QString const audioTemplate = "audio/speakers/%1/%2.ogg";
        return MarbleDirs::path( audioTemplate.arg( m_speaker ).arg( name ) );
    }
}


void AudioOutputPrivate::setupAudio( qreal distance, RoutingInstruction::TurnType turnType )
{
    if ( !m_output ) {
        m_output = new Phonon::MediaObject( q );
        Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput( Phonon::VideoCategory, q );
        Phonon::createPath( m_output, audioOutput );
    }

    QString distanceAudio = distanceAudioFile( distance );
    QString turnTypeAudio = turnTypeAudioFile( turnType );
    if ( turnTypeAudio.isEmpty() ) {
        mDebug() << "Missing audio file for turn type " << turnType;
        return;
    }

    m_output->enqueue( turnTypeAudio );
    if ( !distanceAudio.isEmpty() ) {
        m_output->enqueue( audioFile( "After" ) );
        m_output->enqueue( distanceAudio );
        m_output->enqueue( audioFile( "Meters" ) );
    }

    q->connect( m_output, SIGNAL( finished() ), q, SLOT( audioOutputFinished() ) );
}

void AudioOutputPrivate::reset()
{
    if ( m_output ) {
        m_output->stop();
        m_output->setCurrentSource( QString() );
        m_output->clearQueue();
    }

    m_lastDistance = 0.0;
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

void AudioOutput::update( int index, qreal distance, RoutingInstruction::TurnType turnType )
{
    if ( d->m_muted ) {
        return;
    }

    if ( index != d->m_lastIndex || turnType != d->m_lastTurnType ) {
        d->reset();
    }

    if ( ( d->m_lastDistance == 0 || d->m_lastDistance > 850 ) && distance <= 850 ) {
        if ( !d->m_output || d->m_output->currentSource().fileName().isEmpty() ) {
            d->setupAudio( distance, turnType );
            if ( d->m_output ) {
                d->m_output->play();
            }
        }
    }

    d->m_lastIndex = index;
    d->m_lastTurnType = turnType;
    d->m_lastDistance = distance;
}

void AudioOutput::setMuted( bool muted )
{
    d->m_muted = muted;
}

void AudioOutput::setSpeaker( const QString &speaker )
{
    d->m_speaker = speaker;
}

QStringList AudioOutput::speakers() const
{
    QString const voicePath = MarbleDirs::path( "audio/speakers" );
    if ( !voicePath.isEmpty() ) {
        QDir::Filters filter = QDir::Readable | QDir::Dirs | QDir::NoDotAndDotDot;
        QDir voiceDir( voicePath );
        return voiceDir.entryList( filter, QDir::Name );
    }

    return QStringList();
}

void AudioOutput::setSoundEnabled( bool enabled )
{
    d->m_soundEnabled = enabled;
    d->m_turnTypeMap.clear();

    if ( enabled ) {
        d->m_turnTypeMap[RoutingInstruction::Straight] = "KDE-Sys-App-Positive";
        d->m_turnTypeMap[RoutingInstruction::SlightRight] = "KDE-Sys-App-Positive";
        d->m_turnTypeMap[RoutingInstruction::Right] = "KDE-Sys-App-Positive";
        d->m_turnTypeMap[RoutingInstruction::SharpRight] = "KDE-Sys-App-Positive";
        d->m_turnTypeMap[RoutingInstruction::TurnAround] = "KDE-Sys-App-Positive";
        d->m_turnTypeMap[RoutingInstruction::SharpLeft] = "KDE-Sys-App-Positive";
        d->m_turnTypeMap[RoutingInstruction::Left] = "KDE-Sys-App-Positive";
        d->m_turnTypeMap[RoutingInstruction::SlightLeft] = "KDE-Sys-App-Positive";
        d->m_turnTypeMap[RoutingInstruction::RoundaboutFirstExit] = "KDE-Sys-App-Positive";
        d->m_turnTypeMap[RoutingInstruction::RoundaboutSecondExit] = "KDE-Sys-App-Positive";
        d->m_turnTypeMap[RoutingInstruction::RoundaboutThirdExit] = "KDE-Sys-App-Positive";
    } else {
        d->m_turnTypeMap[RoutingInstruction::Straight] = "Straight";
        d->m_turnTypeMap[RoutingInstruction::SlightRight] = "BearRight";
        d->m_turnTypeMap[RoutingInstruction::Right] = "TurnRight";
        d->m_turnTypeMap[RoutingInstruction::SharpRight] = "SharpRight";
        d->m_turnTypeMap[RoutingInstruction::TurnAround] = "UTurn";
        d->m_turnTypeMap[RoutingInstruction::SharpLeft] = "SharpLeft";
        d->m_turnTypeMap[RoutingInstruction::Left] = "TurnLeft";
        d->m_turnTypeMap[RoutingInstruction::SlightLeft] = "BearLeft";
        d->m_turnTypeMap[RoutingInstruction::RoundaboutFirstExit] = "RbExit1";
        d->m_turnTypeMap[RoutingInstruction::RoundaboutSecondExit] = "RbExit2";
        d->m_turnTypeMap[RoutingInstruction::RoundaboutThirdExit] = "RbExit3";
    }
}

}

#include "AudioOutput.moc"
