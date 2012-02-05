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

    bool m_destinationReached;

    bool m_deviated;

    VoiceNavigationModelPrivate( VoiceNavigationModel* parent );

    void reset();

    QString audioFile(const QString &name);

    QString distanceAudioFile( qreal dest );

    QString turnTypeAudioFile( Maneuver::Direction turnType, qreal distance );

    void updateInstruction( qreal distance, Maneuver::Direction turnType );

    void updateInstruction( const QString &name );

    void initializeMaps();
};

VoiceNavigationModelPrivate::VoiceNavigationModelPrivate( VoiceNavigationModel* parent ) :
    m_parent( parent ),
    m_speakerEnabled( true ),
    m_gpsStatus( PositionProviderStatusUnavailable ),
    m_lastDistance( 0.0 ),
    m_lastTurnType( Maneuver::Unknown ),
    m_destinationReached( false ),
    m_deviated( false )
{
    initializeMaps();
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
        mDebug() << "Missing audio file for turn type " << turnType << " and speaker " << m_speaker;
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

void VoiceNavigationModelPrivate::initializeMaps()
{
    m_turnTypeMap.clear();
    m_announceMap.clear();

    if ( m_speakerEnabled ) {
        m_announceMap[Maneuver::Straight] = "";
        m_announceMap[Maneuver::SlightRight] = "AhKeepRight";
        m_announceMap[Maneuver::Right] = "AhRightTurn";
        m_announceMap[Maneuver::SharpRight] = "AhRightTurn";
        m_announceMap[Maneuver::TurnAround] = "AhUTurn";
        m_announceMap[Maneuver::SharpLeft] = "AhLeftTurn";
        m_announceMap[Maneuver::Left] = "AhLeftTurn";
        m_announceMap[Maneuver::SlightLeft] = "AhKeepLeft";
        m_announceMap[Maneuver::RoundaboutFirstExit] = "RbExit1";
        m_announceMap[Maneuver::RoundaboutSecondExit] = "RbExit2";
        m_announceMap[Maneuver::RoundaboutThirdExit] = "RbExit3";

        m_turnTypeMap[Maneuver::Straight] = "Straight";
        m_turnTypeMap[Maneuver::SlightRight] = "BearRight";
        m_turnTypeMap[Maneuver::Right] = "TurnRight";
        m_turnTypeMap[Maneuver::SharpRight] = "SharpRight";
        m_turnTypeMap[Maneuver::TurnAround] = "UTurn";
        m_turnTypeMap[Maneuver::SharpLeft] = "SharpLeft";
        m_turnTypeMap[Maneuver::Left] = "TurnLeft";
        m_turnTypeMap[Maneuver::SlightLeft] = "BearLeft";
        m_turnTypeMap[Maneuver::RoundaboutFirstExit] = "";
        m_turnTypeMap[Maneuver::RoundaboutSecondExit] = "";
        m_turnTypeMap[Maneuver::RoundaboutThirdExit] = "";
    } else {
        m_announceMap[Maneuver::Straight] = "KDE-Sys-List-End";
        m_announceMap[Maneuver::SlightRight] = "KDE-Sys-List-End";
        m_announceMap[Maneuver::Right] = "KDE-Sys-List-End";
        m_announceMap[Maneuver::SharpRight] = "KDE-Sys-List-End";
        m_announceMap[Maneuver::TurnAround] = "KDE-Sys-List-End";
        m_announceMap[Maneuver::SharpLeft] = "KDE-Sys-List-End";
        m_announceMap[Maneuver::Left] = "KDE-Sys-List-End";
        m_announceMap[Maneuver::SlightLeft] = "KDE-Sys-List-End";
        m_announceMap[Maneuver::RoundaboutFirstExit] = "KDE-Sys-List-End";
        m_announceMap[Maneuver::RoundaboutSecondExit] = "KDE-Sys-List-End";
        m_announceMap[Maneuver::RoundaboutThirdExit] = "KDE-Sys-List-End";

        m_turnTypeMap[Maneuver::Straight] = "KDE-Sys-App-Positive";
        m_turnTypeMap[Maneuver::SlightRight] = "KDE-Sys-App-Positive";
        m_turnTypeMap[Maneuver::Right] = "KDE-Sys-App-Positive";
        m_turnTypeMap[Maneuver::SharpRight] = "KDE-Sys-App-Positive";
        m_turnTypeMap[Maneuver::TurnAround] = "KDE-Sys-App-Positive";
        m_turnTypeMap[Maneuver::SharpLeft] = "KDE-Sys-App-Positive";
        m_turnTypeMap[Maneuver::Left] = "KDE-Sys-App-Positive";
        m_turnTypeMap[Maneuver::SlightLeft] = "KDE-Sys-App-Positive";
        m_turnTypeMap[Maneuver::RoundaboutFirstExit] = "KDE-Sys-App-Positive";
        m_turnTypeMap[Maneuver::RoundaboutSecondExit] = "KDE-Sys-App-Positive";
        m_turnTypeMap[Maneuver::RoundaboutThirdExit] = "KDE-Sys-App-Positive";
    }
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
        emit previewChanged();
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
        d->initializeMaps();
        emit isSpeakerEnabledChanged();
        emit previewChanged();
    }
}

void VoiceNavigationModel::reset()
{
    d->reset();
}

void VoiceNavigationModel::handleTrackingStatusChange( PositionProviderStatus status )
{
    if ( status != PositionProviderStatusAvailable && d->m_gpsStatus == PositionProviderStatusAvailable ) {
        d->updateInstruction( d->m_speakerEnabled ? "GpsLost" : "KDE-Sys-List-End" );
    }

    if ( status == PositionProviderStatusAvailable && d->m_gpsStatus != PositionProviderStatusAvailable ) {
        d->updateInstruction( d->m_speakerEnabled ? "GpsFound" : "KDE-Sys-App-Positive" );
    }

    d->m_gpsStatus = status;
}

void VoiceNavigationModel::update(const Route &route, qreal distanceManuever, qreal distanceTarget, bool deviated )
{
    if ( d->m_destinationReached && distanceTarget < 250 ) {
        return;
    }

    if ( !d->m_destinationReached && distanceTarget < 50 ) {
        d->m_destinationReached = true;
        d->updateInstruction( d->m_speakerEnabled ? "Arrive" : "KDE-Sys-App-Positive" );
        return;
    }

    if ( distanceTarget > 150 ) {
        d->m_destinationReached = false;
    }

    if ( deviated && !d->m_deviated ) {
        d->updateInstruction( d->m_speakerEnabled ? "RouteDeviated" : "KDE-Sys-List-End" );
    }
    d->m_deviated = deviated;
    if ( deviated ) {
        return;
    }

    Maneuver::Direction turnType = route.currentSegment().nextRouteSegment().maneuver().direction();
    if ( !( d->m_lastTurnPoint == route.currentSegment().nextRouteSegment().maneuver().position() ) || turnType != d->m_lastTurnType ) {
        d->m_lastTurnPoint = route.currentSegment().nextRouteSegment().maneuver().position();
        d->reset();
    }

    bool const announcement = ( d->m_lastDistance == 0 || d->m_lastDistance > 850 ) && distanceManuever <= 850;
    bool const turn = ( d->m_lastDistance == 0 || d->m_lastDistance > 75 ) && distanceManuever <= 75;
    if ( announcement || turn ) {
        d->updateInstruction( distanceManuever, turnType );
    }

    d->m_lastTurnType = turnType;
    d->m_lastDistance = distanceManuever;
}

QString VoiceNavigationModel::preview() const
{
    return d->audioFile( d->m_speakerEnabled ? "Marble" : "KDE-Sys-App-Positive" );
}

QString VoiceNavigationModel::instruction() const
{
    return d->m_queue.first();
}

}

#include "VoiceNavigationModel.moc"
