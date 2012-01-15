//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "VoiceNavigationModel.h"

#include "MarbleDirs.h"
#include "MarbleDebug.h"

namespace Marble
{

class VoiceNavigationModelPrivate
{
public:
    VoiceNavigationModel* m_parent;

    QString m_speaker;

    bool m_speakerEnabled;

    PositionProviderStatus m_gpsStatus;

    QMap<Maneuver::Direction, QString> m_turnTypeMap;

    QMap<Maneuver::Direction, QString> m_announceMap;

    qreal m_lastDistance;

    Maneuver::Direction m_lastTurnType;

    GeoDataCoordinates m_lastTurnPoint;

    QStringList m_queue;

    VoiceNavigationModelPrivate( VoiceNavigationModel* parent );

    void reset();

    QString audioFile(const QString &name);

    QString distanceAudioFile( qreal dest );

    QString turnTypeAudioFile( Maneuver::Direction turnType, qreal distance );

    void updateInstruction( qreal distance, Maneuver::Direction turnType );

    void updateInstruction( const QString &name );
};

VoiceNavigationModelPrivate::VoiceNavigationModelPrivate( VoiceNavigationModel* parent ) :
    m_parent( parent ),
    m_speakerEnabled( true ),
    m_gpsStatus( PositionProviderStatusUnavailable ),
    m_lastDistance( 0.0 ),
    m_lastTurnType( Maneuver::Unknown )
{
    // nothing to do
}

void VoiceNavigationModelPrivate::reset()
{
    m_lastDistance = 0.0;
}

QString VoiceNavigationModelPrivate::audioFile( const QString &name )
{
    QStringList const formats = QStringList() << "ogg" << "mp3" << "wav";
    if ( !m_speakerEnabled ) {
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

QString VoiceNavigationModelPrivate::distanceAudioFile( qreal dest )
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

QString VoiceNavigationModelPrivate::turnTypeAudioFile( Maneuver::Direction turnType, qreal distance )
{
    QMap<Maneuver::Direction, QString> const & map = distance < 75 ? m_turnTypeMap : m_announceMap;
    if ( map.contains( turnType ) ) {
        return audioFile( map[turnType] );
    }

    return QString();
}

void VoiceNavigationModelPrivate::updateInstruction( qreal distance, Maneuver::Direction turnType )
{
    //QString distanceAudio = distanceAudioFile( distance );
    QString turnTypeAudio = turnTypeAudioFile( turnType, distance );
    if ( turnTypeAudio.isEmpty() ) {
        mDebug() << "Missing audio file for turn type " << turnType;
        return;
    }

    m_queue.clear();
    m_queue << turnTypeAudio;
    emit m_parent->instructionChanged();
//    if ( !distanceAudio.isEmpty() ) {
//        m_output->enqueue( audioFile( "After" ) );
//        m_output->enqueue( distanceAudio );
//        m_output->enqueue( audioFile( "Meters" ) );
    //    }
}

void VoiceNavigationModelPrivate::updateInstruction( const QString &name )
{
    m_queue.clear();
    m_queue << audioFile( name );
    emit m_parent->instructionChanged();
}

VoiceNavigationModel::VoiceNavigationModel( QObject *parent ) :
    QObject( parent ), d( new VoiceNavigationModelPrivate( this ) )
{
    // nothing to do
}

VoiceNavigationModel::~VoiceNavigationModel()
{
    delete d;
}

QString VoiceNavigationModel::speaker() const
{
    return d->m_speaker;
}

void VoiceNavigationModel::setSpeaker(const QString &speaker)
{
    if ( speaker != d->m_speaker ) {
        QFileInfo speakerDir = QFileInfo( speaker );
        if ( !speakerDir.exists() ) {
            d->m_speaker = MarbleDirs::path( "/audio/speakers/" + speaker );
        } else {
            d->m_speaker = speaker;
        }

        emit speakerChanged();
    }
}

bool VoiceNavigationModel::isSpeakerEnabled() const
{
    return d->m_speakerEnabled;
}

void VoiceNavigationModel::setSpeakerEnabled( bool enabled )
{
    if ( enabled != d->m_speakerEnabled ) {
        d->m_speakerEnabled = enabled;

        d->m_turnTypeMap.clear();
        d->m_announceMap.clear();

        if ( enabled ) {
            d->m_announceMap[Maneuver::Straight] = "";
            d->m_announceMap[Maneuver::SlightRight] = "AhKeepRight";
            d->m_announceMap[Maneuver::Right] = "AhRightTurn";
            d->m_announceMap[Maneuver::SharpRight] = "AhRightTurn";
            d->m_announceMap[Maneuver::TurnAround] = "AhUTurn";
            d->m_announceMap[Maneuver::SharpLeft] = "AhLeftTurn";
            d->m_announceMap[Maneuver::Left] = "AhLeftTurn";
            d->m_announceMap[Maneuver::SlightLeft] = "AhKeepLeft";
            d->m_announceMap[Maneuver::RoundaboutFirstExit] = "RbExit1";
            d->m_announceMap[Maneuver::RoundaboutSecondExit] = "RbExit2";
            d->m_announceMap[Maneuver::RoundaboutThirdExit] = "RbExit3";

            d->m_turnTypeMap[Maneuver::Straight] = "Straight";
            d->m_turnTypeMap[Maneuver::SlightRight] = "BearRight";
            d->m_turnTypeMap[Maneuver::Right] = "TurnRight";
            d->m_turnTypeMap[Maneuver::SharpRight] = "SharpRight";
            d->m_turnTypeMap[Maneuver::TurnAround] = "UTurn";
            d->m_turnTypeMap[Maneuver::SharpLeft] = "SharpLeft";
            d->m_turnTypeMap[Maneuver::Left] = "TurnLeft";
            d->m_turnTypeMap[Maneuver::SlightLeft] = "BearLeft";
            d->m_turnTypeMap[Maneuver::RoundaboutFirstExit] = "";
            d->m_turnTypeMap[Maneuver::RoundaboutSecondExit] = "";
            d->m_turnTypeMap[Maneuver::RoundaboutThirdExit] = "";
        } else {
            d->m_announceMap[Maneuver::Straight] = "KDE-Sys-List-End";
            d->m_announceMap[Maneuver::SlightRight] = "KDE-Sys-List-End";
            d->m_announceMap[Maneuver::Right] = "KDE-Sys-List-End";
            d->m_announceMap[Maneuver::SharpRight] = "KDE-Sys-List-End";
            d->m_announceMap[Maneuver::TurnAround] = "KDE-Sys-List-End";
            d->m_announceMap[Maneuver::SharpLeft] = "KDE-Sys-List-End";
            d->m_announceMap[Maneuver::Left] = "KDE-Sys-List-End";
            d->m_announceMap[Maneuver::SlightLeft] = "KDE-Sys-List-End";
            d->m_announceMap[Maneuver::RoundaboutFirstExit] = "KDE-Sys-List-End";
            d->m_announceMap[Maneuver::RoundaboutSecondExit] = "KDE-Sys-List-End";
            d->m_announceMap[Maneuver::RoundaboutThirdExit] = "KDE-Sys-List-End";

            d->m_turnTypeMap[Maneuver::Straight] = "KDE-Sys-App-Positive";
            d->m_turnTypeMap[Maneuver::SlightRight] = "KDE-Sys-App-Positive";
            d->m_turnTypeMap[Maneuver::Right] = "KDE-Sys-App-Positive";
            d->m_turnTypeMap[Maneuver::SharpRight] = "KDE-Sys-App-Positive";
            d->m_turnTypeMap[Maneuver::TurnAround] = "KDE-Sys-App-Positive";
            d->m_turnTypeMap[Maneuver::SharpLeft] = "KDE-Sys-App-Positive";
            d->m_turnTypeMap[Maneuver::Left] = "KDE-Sys-App-Positive";
            d->m_turnTypeMap[Maneuver::SlightLeft] = "KDE-Sys-App-Positive";
            d->m_turnTypeMap[Maneuver::RoundaboutFirstExit] = "KDE-Sys-App-Positive";
            d->m_turnTypeMap[Maneuver::RoundaboutSecondExit] = "KDE-Sys-App-Positive";
            d->m_turnTypeMap[Maneuver::RoundaboutThirdExit] = "KDE-Sys-App-Positive";
        }
    }
}

void VoiceNavigationModel::reset()
{
    d->reset();
}

void VoiceNavigationModel::handleTrackingStatusChange( PositionProviderStatus status )
{
    if ( status != PositionProviderStatusAvailable && d->m_gpsStatus == PositionProviderStatusAvailable ) {
        /** @todo: Announce gps connect lost here */
    }

    d->m_gpsStatus = status;
}

void VoiceNavigationModel::update(const Route &route, qreal distance)
{
    Maneuver::Direction turnType = route.currentSegment().nextRouteSegment().maneuver().direction();
    if ( !( d->m_lastTurnPoint == route.currentSegment().nextRouteSegment().maneuver().position() ) || turnType != d->m_lastTurnType ) {
        d->m_lastTurnPoint = route.currentSegment().nextRouteSegment().maneuver().position();
        d->reset();
    }

    bool const announcement = ( d->m_lastDistance == 0 || d->m_lastDistance > 850 ) && distance <= 850;
    bool const turn = ( d->m_lastDistance == 0 || d->m_lastDistance > 75 ) && distance <= 75;
    if ( announcement || turn ) {
        d->updateInstruction( distance, turnType );
    }

    d->m_lastTurnType = turnType;
    d->m_lastDistance = distance;
}

void VoiceNavigationModel::setDestinationReached()
{
    d->updateInstruction( d->m_speakerEnabled ? "Arrive" : "KDE-Sys-App-Positive" );
}

QString VoiceNavigationModel::preview() const
{
    return d->audioFile( "Marble" );
}

QString VoiceNavigationModel::instruction() const
{
    return d->m_queue.first();
}

}

#include "VoiceNavigationModel.moc"
