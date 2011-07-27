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

    QMap<RoutingInstruction::TurnType, QString> m_announceMap;

    QString m_speaker;

    Phonon::MediaObject *m_output;

    qreal m_lastDistance;

    RoutingInstruction::TurnType m_lastTurnType;

    bool m_muted;

    bool m_soundEnabled;

    GeoDataCoordinates m_lastTurnPoint;

    AudioOutputPrivate( AudioOutput* parent );

    void audioOutputFinished();

    void setupAudio();

    QString distanceAudioFile( qreal distance );

    QString turnTypeAudioFile( RoutingInstruction::TurnType turnType, qreal distance );

    QString audioFile( const QString &name );

    void reset();

    void enqueue( qreal distance, RoutingInstruction::TurnType turnType );

    void enqueue( const QString &file );
};

AudioOutputPrivate::AudioOutputPrivate( AudioOutput* parent ) :
    q( parent ), m_output( 0 ), m_lastDistance( 0.0 ),
    m_muted( false ), m_soundEnabled( true )
{
    // nothing to do
}

void AudioOutputPrivate::audioOutputFinished()
{
    m_output->setCurrentSource( QString() );
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

QString AudioOutputPrivate::turnTypeAudioFile( RoutingInstruction::TurnType turnType, qreal distance )
{
    QMap<RoutingInstruction::TurnType, QString> const & map = distance < 75 ? m_turnTypeMap : m_announceMap;
    if ( map.contains( turnType ) ) {
        return audioFile( map[turnType] );
    }

    return QString();
}

QString AudioOutputPrivate::audioFile( const QString &name )
{
    QStringList const formats = QStringList() << "ogg" << "mp3" << "wav";
    if ( m_soundEnabled ) {
        QString const audioTemplate = "audio/%1.%2";
        foreach( const QString &format, formats ) {
            QString const result = MarbleDirs::path( audioTemplate.arg( name ).arg( format ) );
            if ( !result.isEmpty() ) {
                return result;
            }
        }
    } else {
        QString const audioTemplate = "%1/%2.%3";
        foreach( const QString &format, formats ) {
            QString const result = audioTemplate.arg( m_speaker ).arg( name ).arg( format );
            QFileInfo audioFile( result );
            if ( audioFile.exists() ) {
                return result;
            }
        }
    }

    return QString();
}

void AudioOutputPrivate::setupAudio()
{
    if ( !m_output ) {
        m_output = new Phonon::MediaObject( q );
        Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput( Phonon::VideoCategory, q );
        Phonon::createPath( m_output, audioOutput );

        q->connect( m_output, SIGNAL( finished() ), q, SLOT( audioOutputFinished() ) );
    }
}

void AudioOutputPrivate::enqueue( qreal distance, RoutingInstruction::TurnType turnType )
{
    if ( !m_output ) {
        return;
    }

    //QString distanceAudio = distanceAudioFile( distance );
    QString turnTypeAudio = turnTypeAudioFile( turnType, distance );
    if ( turnTypeAudio.isEmpty() ) {
        mDebug() << "Missing audio file for turn type " << turnType;
        return;
    }

    m_output->enqueue( turnTypeAudio );
//    if ( !distanceAudio.isEmpty() ) {
//        m_output->enqueue( audioFile( "After" ) );
//        m_output->enqueue( distanceAudio );
//        m_output->enqueue( audioFile( "Meters" ) );
//    }
}

void AudioOutputPrivate::enqueue( const QString &file )
{
    if ( m_output ) {
        m_output->enqueue( audioFile( file ) );
    }
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

void AudioOutput::update( const Route &route, qreal distance )
{
    if ( d->m_muted ) {
        return;
    }

    RoutingInstruction::TurnType turnType = route.currentSegment().nextRouteSegment().maneuver().direction();
    if ( !( d->m_lastTurnPoint == route.currentSegment().nextRouteSegment().maneuver().position() ) || turnType != d->m_lastTurnType ) {
        d->m_lastTurnPoint = route.currentSegment().nextRouteSegment().maneuver().position();
        d->reset();
    }

    bool const announcement = ( d->m_lastDistance == 0 || d->m_lastDistance > 850 ) && distance <= 850;
    bool const turn = ( d->m_lastDistance == 0 || d->m_lastDistance > 75 ) && distance <= 75;
    if ( announcement || turn ) {
        if ( !d->m_output || d->m_output->currentSource().fileName().isEmpty() ) {
            d->setupAudio();
            d->enqueue( distance, turnType );
            if ( d->m_output ) {
                d->m_output->play();
            }
        }
    }

    d->m_lastTurnType = turnType;
    d->m_lastDistance = distance;
}

void AudioOutput::setMuted( bool muted )
{
    d->m_muted = muted;
}

void AudioOutput::setSpeaker( const QString &speaker )
{
    QFileInfo speakerDir = QFileInfo( speaker );
    if ( !speakerDir.exists() ) {
        d->m_speaker = MarbleDirs::path( "/audio/speakers/" + speaker );
    } else {
        d->m_speaker = speaker;
    }
}

QString AudioOutput::speaker() const
{
    return d->m_speaker;
}

QStringList AudioOutput::speakers() const
{
    QStringList result;
    QStringList const baseDirs = QStringList() << MarbleDirs::systemPath() << MarbleDirs::localPath();
    foreach ( const QString &baseDir, baseDirs ) {
        QString base = baseDir + "/audio/speakers/";

        QDir::Filters filter = QDir::Readable | QDir::Dirs | QDir::NoDotAndDotDot;
        QFileInfoList const speakers = QDir( base ).entryInfoList( filter, QDir::Name );
        foreach( const QFileInfo &speaker, speakers ) {
            result << speaker.absoluteFilePath();
        }
    }

    return result;
}

void AudioOutput::setSoundEnabled( bool enabled )
{
    d->m_soundEnabled = enabled;
    d->m_turnTypeMap.clear();
    d->m_announceMap.clear();

    if ( enabled ) {
        d->m_announceMap[RoutingInstruction::Straight] = "KDE-Sys-List-End";
        d->m_announceMap[RoutingInstruction::SlightRight] = "KDE-Sys-List-End";
        d->m_announceMap[RoutingInstruction::Right] = "KDE-Sys-List-End";
        d->m_announceMap[RoutingInstruction::SharpRight] = "KDE-Sys-List-End";
        d->m_announceMap[RoutingInstruction::TurnAround] = "KDE-Sys-List-End";
        d->m_announceMap[RoutingInstruction::SharpLeft] = "KDE-Sys-List-End";
        d->m_announceMap[RoutingInstruction::Left] = "KDE-Sys-List-End";
        d->m_announceMap[RoutingInstruction::SlightLeft] = "KDE-Sys-List-End";
        d->m_announceMap[RoutingInstruction::RoundaboutFirstExit] = "KDE-Sys-List-End";
        d->m_announceMap[RoutingInstruction::RoundaboutSecondExit] = "KDE-Sys-List-End";
        d->m_announceMap[RoutingInstruction::RoundaboutThirdExit] = "KDE-Sys-List-End";

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
        d->m_announceMap[RoutingInstruction::Straight] = "";
        d->m_announceMap[RoutingInstruction::SlightRight] = "AhKeepRight";
        d->m_announceMap[RoutingInstruction::Right] = "AhRightTurn";
        d->m_announceMap[RoutingInstruction::SharpRight] = "AhRightTurn";
        d->m_announceMap[RoutingInstruction::TurnAround] = "AhUTurn";
        d->m_announceMap[RoutingInstruction::SharpLeft] = "AhLeftTurn";
        d->m_announceMap[RoutingInstruction::Left] = "AhLeftTurn";
        d->m_announceMap[RoutingInstruction::SlightLeft] = "AhKeepLeft";
        d->m_announceMap[RoutingInstruction::RoundaboutFirstExit] = "RbExit1";
        d->m_announceMap[RoutingInstruction::RoundaboutSecondExit] = "RbExit2";
        d->m_announceMap[RoutingInstruction::RoundaboutThirdExit] = "RbExit3";

        d->m_turnTypeMap[RoutingInstruction::Straight] = "Straight";
        d->m_turnTypeMap[RoutingInstruction::SlightRight] = "BearRight";
        d->m_turnTypeMap[RoutingInstruction::Right] = "TurnRight";
        d->m_turnTypeMap[RoutingInstruction::SharpRight] = "SharpRight";
        d->m_turnTypeMap[RoutingInstruction::TurnAround] = "UTurn";
        d->m_turnTypeMap[RoutingInstruction::SharpLeft] = "SharpLeft";
        d->m_turnTypeMap[RoutingInstruction::Left] = "TurnLeft";
        d->m_turnTypeMap[RoutingInstruction::SlightLeft] = "BearLeft";
        d->m_turnTypeMap[RoutingInstruction::RoundaboutFirstExit] = "";
        d->m_turnTypeMap[RoutingInstruction::RoundaboutSecondExit] = "";
        d->m_turnTypeMap[RoutingInstruction::RoundaboutThirdExit] = "";
    }
}

void AudioOutput::announceStart()
{
    if ( d->m_muted || d->m_soundEnabled ) {
        return;
    }

    d->setupAudio();
//    d->enqueue( "Depart" );
//    if ( d->m_output ) {
//        d->m_output->play();
//    }
}

void AudioOutput::announceDestination()
{
    if ( d->m_muted ) {
        return;
    }

    d->setupAudio();
    d->enqueue( d->m_soundEnabled ? "KDE-Sys-App-Positive" : "Arrive" );
    if ( d->m_output ) {
        d->m_output->play();
    }
}

}

#include "AudioOutput.moc"
