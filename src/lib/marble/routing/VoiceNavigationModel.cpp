// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include "VoiceNavigationModel.h"

#include "Route.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"

namespace Marble
{

class VoiceNavigationModelPrivate
{
public:
    struct Announcement {
        bool announcementDone;
        bool turnInstructionDone;

        Announcement()
        {
            announcementDone = false;
            turnInstructionDone = false;
        }
    };

    VoiceNavigationModel *m_parent = nullptr;

    QString m_speaker;

    bool m_speakerEnabled;

    QMap<Maneuver::Direction, QString> m_turnTypeMap;

    QMap<Maneuver::Direction, QString> m_announceMap;

    qreal m_lastDistance;

    qreal m_lastDistanceTraversed;

    GeoDataLineString m_lastRoutePath;

    Maneuver::Direction m_lastTurnType;

    GeoDataCoordinates m_lastTurnPoint;

    QStringList m_queue;

    QString m_announcementText;

    bool m_destinationReached;

    bool m_deviated;

    QList<Announcement> m_announcementList;

    explicit VoiceNavigationModelPrivate(VoiceNavigationModel *parent);

    void reset();

    QString audioFile(const QString &name) const;

    QString distanceAudioFile(qreal dest) const;

    QString turnTypeAudioFile(Maneuver::Direction turnType, qreal distance);

    QString announcementText(Maneuver::Direction turnType, qreal distance);

    void updateInstruction(const RouteSegment &segment, qreal distance, Maneuver::Direction turnType);

    void updateInstruction(const QString &name);

    void initializeMaps();
};

VoiceNavigationModelPrivate::VoiceNavigationModelPrivate(VoiceNavigationModel *parent)
    : m_parent(parent)
    , m_speakerEnabled(true)
    , m_lastDistance(0.0)
    , m_lastDistanceTraversed(0.0)
    , m_lastTurnType(Maneuver::Unknown)
    , m_destinationReached(false)
    , m_deviated(false)
{
    initializeMaps();
}

void VoiceNavigationModelPrivate::reset()
{
    m_lastDistance = 0.0;
    m_lastDistanceTraversed = 0.0;
}

QString VoiceNavigationModelPrivate::audioFile(const QString &name) const
{
#ifdef Q_OS_ANDROID
    return name;
#else
    QStringList const formats = QStringList() << QStringLiteral("ogg") << QStringLiteral("mp3") << QStringLiteral("wav");
    if (m_speakerEnabled) {
        QString const audioTemplate = QStringLiteral("%1/%2.%3");
        for (const QString &format : formats) {
            QString const result = audioTemplate.arg(m_speaker, name, format);
            QFileInfo audioFile(result);
            if (audioFile.exists()) {
                return result;
            }
        }
    }

    QString const audioTemplate = QStringLiteral("audio/%1.%2");
    for (const QString &format : formats) {
        QString const result = MarbleDirs::path(audioTemplate.arg(name, format));
        if (!result.isEmpty()) {
            return result;
        }
    }

    return {};
#endif
}

QString VoiceNavigationModelPrivate::distanceAudioFile(qreal dest) const
{
    if (dest > 0.0 && dest < 900.0) {
        qreal minDistance = 0.0;
        int targetDistance = 0;
        QList<int> distances;
        distances << 50 << 80 << 100 << 200 << 300 << 400 << 500 << 600 << 700 << 800;
        for (int distance : std::as_const(distances)) {
            QString file = audioFile(QString::number(distance));
            qreal currentDistance = qAbs(distance - dest);
            if (!file.isEmpty() && (minDistance == 0.0 || currentDistance < minDistance)) {
                minDistance = currentDistance;
                targetDistance = distance;
            }
        }

        if (targetDistance > 0) {
            return audioFile(QString::number(targetDistance));
        }
    }

    return {};
}

QString VoiceNavigationModelPrivate::turnTypeAudioFile(Maneuver::Direction turnType, qreal distance)
{
    bool const announce = distance >= 75;
    QMap<Maneuver::Direction, QString> const &map = announce ? m_announceMap : m_turnTypeMap;
    if (m_speakerEnabled && map.contains(turnType)) {
        return audioFile(map[turnType]);
    }

    return audioFile(announce ? QStringLiteral("ListEnd") : QStringLiteral("AppPositive"));
}

QString VoiceNavigationModelPrivate::announcementText(Maneuver::Direction turnType, qreal distance)
{
    QString announcementText;
    if (distance >= 75) {
        announcementText = QString(QStringLiteral("In ") + distanceAudioFile(distance) + QStringLiteral(" meters, "));
    }
    switch (turnType) {
    case Maneuver::Continue:
    case Maneuver::Straight:
        announcementText += QStringLiteral("Continue straight");
        break;
    case Maneuver::SlightRight:
        announcementText += QStringLiteral("Turn slight right");
        break;
    case Maneuver::SlightLeft:
        announcementText += QStringLiteral("Turn slight left");
        break;
    case Maneuver::Right:
    case Maneuver::SharpRight:
        announcementText += QStringLiteral("Turn right");
        break;
    case Maneuver::Left:
    case Maneuver::SharpLeft:
        announcementText += QStringLiteral("Turn left");
        break;
    case Maneuver::TurnAround:
        announcementText += QStringLiteral("Take a U-turn");
        break;
    case Maneuver::ExitLeft:
        announcementText += QStringLiteral("Exit left");
        break;
    case Maneuver::ExitRight:
        announcementText += QStringLiteral("Exit right");
        break;
    case Maneuver::RoundaboutFirstExit:
        announcementText += QStringLiteral("Take the first exit");
        break;
    case Maneuver::RoundaboutSecondExit:
        announcementText += QStringLiteral("Take the second exit");
        break;
    case Maneuver::RoundaboutThirdExit:
        announcementText += QStringLiteral("Take the third exit");
        break;
    default:
        announcementText = QStringLiteral("");
        break;
    }
    return announcementText;
}

void VoiceNavigationModelPrivate::updateInstruction(const RouteSegment &segment, qreal distance, Maneuver::Direction turnType)
{
    QString turnTypeAudio = turnTypeAudioFile(turnType, distance);
    if (turnTypeAudio.isEmpty()) {
        mDebug() << "Missing audio file for turn type " << turnType << " and speaker " << m_speaker;
        return;
    }

    m_queue.clear();
    m_queue << turnTypeAudio;
    m_announcementText = announcementText(turnType, distance);
    qreal nextSegmentDistance = segment.nextRouteSegment().distance();
    Maneuver::Direction nextSegmentDirection = segment.nextRouteSegment().nextRouteSegment().maneuver().direction();
    if (!m_announcementText.isEmpty() && distance < 75 && nextSegmentDistance != 0 && nextSegmentDistance < 75) {
        QString nextSegmentAnnouncementText = announcementText(nextSegmentDirection, nextSegmentDistance);
        if (!nextSegmentAnnouncementText.isEmpty()) {
            m_announcementText += QLatin1StringView(", then ") + nextSegmentAnnouncementText;
        }
    }
    Q_EMIT m_parent->instructionChanged();
}

void VoiceNavigationModelPrivate::updateInstruction(const QString &name)
{
    m_queue.clear();
    m_queue << audioFile(name);
    m_announcementText = name;
    Q_EMIT m_parent->instructionChanged();
}

void VoiceNavigationModelPrivate::initializeMaps()
{
    m_turnTypeMap.clear();
    m_announceMap.clear();

    m_announceMap[Maneuver::Continue] = QStringLiteral("Straight");
    // none of our voice navigation commands fits, so leave out
    // Maneuver::Merge here to have a sound play instead
    m_announceMap[Maneuver::Straight] = QStringLiteral("Straight");
    m_announceMap[Maneuver::SlightRight] = QStringLiteral("AhKeepRight");
    m_announceMap[Maneuver::Right] = QStringLiteral("AhRightTurn");
    m_announceMap[Maneuver::SharpRight] = QStringLiteral("AhRightTurn");
    m_announceMap[Maneuver::TurnAround] = QStringLiteral("AhUTurn");
    m_announceMap[Maneuver::SharpLeft] = QStringLiteral("AhLeftTurn");
    m_announceMap[Maneuver::Left] = QStringLiteral("AhLeftTurn");
    m_announceMap[Maneuver::SlightLeft] = QStringLiteral("AhKeepLeft");
    m_announceMap[Maneuver::RoundaboutFirstExit] = QStringLiteral("RbExit1");
    m_announceMap[Maneuver::RoundaboutSecondExit] = QStringLiteral("RbExit2");
    m_announceMap[Maneuver::RoundaboutThirdExit] = QStringLiteral("RbExit3");
    m_announceMap[Maneuver::ExitLeft] = QStringLiteral("AhExitLeft");
    m_announceMap[Maneuver::ExitRight] = QStringLiteral("AhExitRight");

    m_turnTypeMap[Maneuver::Continue] = QStringLiteral("Straight");
    // none of our voice navigation commands fits, so leave out
    // Maneuver::Merge here to have a sound play instead
    m_turnTypeMap[Maneuver::Straight] = QStringLiteral("Straight");
    m_turnTypeMap[Maneuver::SlightRight] = QStringLiteral("BearRight");
    m_turnTypeMap[Maneuver::Right] = QStringLiteral("TurnRight");
    m_turnTypeMap[Maneuver::SharpRight] = QStringLiteral("SharpRight");
    m_turnTypeMap[Maneuver::TurnAround] = QStringLiteral("UTurn");
    m_turnTypeMap[Maneuver::SharpLeft] = QStringLiteral("SharpLeft");
    m_turnTypeMap[Maneuver::Left] = QStringLiteral("TurnLeft");
    m_turnTypeMap[Maneuver::SlightLeft] = QStringLiteral("BearLeft");
    m_turnTypeMap[Maneuver::RoundaboutFirstExit] = QString();
    m_turnTypeMap[Maneuver::RoundaboutSecondExit] = QString();
    m_turnTypeMap[Maneuver::RoundaboutThirdExit] = QString();
    m_turnTypeMap[Maneuver::ExitLeft] = QStringLiteral("TurnLeft");
    m_turnTypeMap[Maneuver::ExitRight] = QStringLiteral("TurnRight");
}

VoiceNavigationModel::VoiceNavigationModel(QObject *parent)
    : QObject(parent)
    , d(new VoiceNavigationModelPrivate(this))
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
    if (speaker != d->m_speaker) {
        QFileInfo speakerDir = QFileInfo(speaker);
        if (!speakerDir.exists()) {
            d->m_speaker = MarbleDirs::path(QLatin1StringView("/audio/speakers/") + speaker);
        } else {
            d->m_speaker = speaker;
        }

        Q_EMIT speakerChanged();
        Q_EMIT previewChanged();
    }
}

bool VoiceNavigationModel::isSpeakerEnabled() const
{
    return d->m_speakerEnabled;
}

void VoiceNavigationModel::setSpeakerEnabled(bool enabled)
{
    if (enabled != d->m_speakerEnabled) {
        d->m_speakerEnabled = enabled;
        Q_EMIT isSpeakerEnabledChanged();
        Q_EMIT previewChanged();
    }
}

void VoiceNavigationModel::reset()
{
    d->reset();
}

void VoiceNavigationModel::update(const Route &route, qreal distanceManuever, qreal distanceTarget, bool deviated)
{
    if (d->m_lastRoutePath != route.path()) {
        d->m_announcementList.clear();
        d->m_announcementList.resize(route.size());
        d->m_lastRoutePath = route.path();
    }

    if (d->m_destinationReached && distanceTarget < 250) {
        return;
    }

    if (!d->m_destinationReached && distanceTarget < 50) {
        d->m_destinationReached = true;
        d->updateInstruction(d->m_speakerEnabled ? QStringLiteral("You have arrived at your destination") : QStringLiteral("AppPositive"));
        return;
    }

    if (distanceTarget > 150) {
        d->m_destinationReached = false;
    }

    if (deviated && !d->m_deviated) {
        d->updateInstruction(d->m_speakerEnabled ? QStringLiteral("Deviated from the route") : QStringLiteral("ListEnd"));
    }
    d->m_deviated = deviated;
    if (deviated) {
        return;
    }

    Maneuver::Direction turnType = route.currentSegment().nextRouteSegment().maneuver().direction();
    if (!(d->m_lastTurnPoint == route.currentSegment().nextRouteSegment().maneuver().position()) || turnType != d->m_lastTurnType) {
        d->m_lastTurnPoint = route.currentSegment().nextRouteSegment().maneuver().position();
        d->reset();
    }

    qreal const distanceTraversed = route.currentSegment().distance() - distanceManuever;
    bool const minDistanceTraversed = d->m_lastDistanceTraversed < 40 && distanceTraversed >= 40;
    bool const announcementAfterTurn = minDistanceTraversed && distanceManuever >= 75;
    bool const announcement = (d->m_lastDistance > 850 || announcementAfterTurn) && distanceManuever <= 850;
    bool const turn = (d->m_lastDistance == 0 || d->m_lastDistance > 75) && distanceManuever <= 75;

    if (route.size() > 0) {
        int const index = route.indexOf(route.currentSegment());
        bool const announcementDone = d->m_announcementList[index].announcementDone;
        bool const turnInstructionDone = d->m_announcementList[index].turnInstructionDone;

        if ((announcement && !announcementDone) || (turn && !turnInstructionDone)) {
            d->updateInstruction(route.currentSegment(), distanceManuever, turnType);
            VoiceNavigationModelPrivate::Announcement &curAnnouncement = d->m_announcementList[index];
            if (announcement) {
                curAnnouncement.announcementDone = true;
            }
            if (turn) {
                curAnnouncement.turnInstructionDone = true;
            }
        }
    }

    d->m_lastTurnType = turnType;
    d->m_lastDistance = distanceManuever;
    d->m_lastDistanceTraversed = distanceTraversed;
}

QString VoiceNavigationModel::preview() const
{
    return d->audioFile(d->m_speakerEnabled ? QStringLiteral("The Marble team wishes you a pleasant and safe journey!") : QStringLiteral("AppPositive"));
}

QString VoiceNavigationModel::instruction() const
{
    return d->m_announcementText;
}

}

#include "moc_VoiceNavigationModel.cpp"
