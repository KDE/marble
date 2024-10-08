// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "TourPlayback.h"

#include <QList>
#include <QPointer>
#include <QUrl>

#include "AnimatedUpdateTrack.h"
#include "GeoDataAnimatedUpdate.h"
#include "GeoDataFlyTo.h"
#include "GeoDataLookAt.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPlaylist.h"
#include "GeoDataPoint.h"
#include "GeoDataSoundCue.h"
#include "GeoDataTour.h"
#include "GeoDataTourControl.h"
#include "GeoDataTreeModel.h"
#include "GeoDataWait.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "PlaybackAnimatedUpdateItem.h"
#include "PlaybackFlyToItem.h"
#include "PlaybackSoundCueItem.h"
#include "PlaybackTourControlItem.h"
#include "PlaybackWaitItem.h"
#include "PopupLayer.h"
#include "SerialTrack.h"
#include "SoundTrack.h"

namespace Marble
{

class TourPlaybackPrivate
{
public:
    TourPlaybackPrivate();
    ~TourPlaybackPrivate();

    GeoDataTour *m_tour = nullptr;
    bool m_pause;
    SerialTrack m_mainTrack;
    QList<SoundTrack *> m_soundTracks;
    QList<AnimatedUpdateTrack *> m_animatedUpdateTracks;
    GeoDataFlyTo m_mapCenter;
    QPointer<MarbleWidget> m_widget;
    QUrl m_baseUrl;
};

TourPlaybackPrivate::TourPlaybackPrivate()
    : m_tour(nullptr)
    , m_pause(false)
    , m_mainTrack()
    , m_widget(nullptr)
{
    // do nothing
}

TourPlaybackPrivate::~TourPlaybackPrivate()
{
    qDeleteAll(m_soundTracks);
    qDeleteAll(m_animatedUpdateTracks);
}

TourPlayback::TourPlayback(QObject *parent)
    : QObject(parent)
    , d(new TourPlaybackPrivate())
{
    connect(&d->m_mainTrack, &SerialTrack::centerOn, this, &TourPlayback::centerOn);
    connect(&d->m_mainTrack, &SerialTrack::progressChanged, this, &TourPlayback::progressChanged);
    connect(&d->m_mainTrack, &SerialTrack::finished, this, &TourPlayback::stopTour);
    connect(&d->m_mainTrack, &SerialTrack::itemFinished, this, &TourPlayback::handleFinishedItem);
}

TourPlayback::~TourPlayback()
{
    stop();
    delete d;
}

void TourPlayback::handleFinishedItem(int index)
{
    Q_EMIT itemFinished(index);
}

void TourPlayback::stopTour()
{
    for (SoundTrack *track : std::as_const(d->m_soundTracks)) {
        track->stop();
        track->setPaused(false);
    }
    for (int i = d->m_animatedUpdateTracks.size() - 1; i >= 0; i--) {
        d->m_animatedUpdateTracks[i]->stop();
        d->m_animatedUpdateTracks[i]->setPaused(false);
    }
    Q_EMIT finished();
}

void TourPlayback::showBalloon(GeoDataPlacemark *placemark)
{
    auto point = static_cast<GeoDataPoint *>(placemark->geometry());
    d->m_widget->popupLayer()->setCoordinates(point->coordinates(), Qt::AlignRight | Qt::AlignVCenter);
    d->m_widget->popupLayer()->setContent(placemark->description(), d->m_baseUrl);
    d->m_widget->popupLayer()->setVisible(true);
    d->m_widget->popupLayer()->setSize(QSizeF(500, 520));
}

void TourPlayback::hideBalloon()
{
    if (d->m_widget) {
        d->m_widget->popupLayer()->setVisible(false);
    }
}

bool TourPlayback::isPlaying() const
{
    return !d->m_pause;
}

void TourPlayback::setMarbleWidget(MarbleWidget *widget)
{
    d->m_widget = widget;

    connect(this, &TourPlayback::added, d->m_widget->model()->treeModel(), &GeoDataTreeModel::addFeature);
    connect(this, SIGNAL(removed(GeoDataFeature *)), d->m_widget->model()->treeModel(), SLOT(removeFeature(GeoDataFeature *)));
    connect(this, &TourPlayback::updated, d->m_widget->model()->treeModel(), &GeoDataTreeModel::updateFeature);
}

void TourPlayback::setBaseUrl(const QUrl &baseUrl)
{
    d->m_baseUrl = baseUrl;
}

QUrl TourPlayback::baseUrl() const
{
    return d->m_baseUrl;
}

void TourPlayback::centerOn(const GeoDataCoordinates &coordinates)
{
    if (d->m_widget) {
        GeoDataLookAt lookat;
        lookat.setCoordinates(coordinates);
        lookat.setRange(coordinates.altitude());
        d->m_widget->flyTo(lookat, Instant);
    }
}

void TourPlayback::setTour(GeoDataTour *tour)
{
    d->m_tour = tour;
    if (!d->m_tour) {
        clearTracks();
        return;
    }

    updateTracks();
}

void TourPlayback::play()
{
    d->m_pause = false;
    auto lookat = new GeoDataLookAt(d->m_widget->lookAt());
    lookat->setAltitude(lookat->range());
    d->m_mapCenter.setView(lookat);
    d->m_mainTrack.play();
    for (SoundTrack *track : std::as_const(d->m_soundTracks)) {
        track->play();
    }
    for (AnimatedUpdateTrack *track : std::as_const(d->m_animatedUpdateTracks)) {
        track->play();
    }
}

void TourPlayback::pause()
{
    d->m_pause = true;
    d->m_mainTrack.pause();
    for (SoundTrack *track : std::as_const(d->m_soundTracks)) {
        track->pause();
    }
    for (AnimatedUpdateTrack *track : std::as_const(d->m_animatedUpdateTracks)) {
        track->pause();
    }
}

void TourPlayback::stop()
{
    d->m_pause = true;
    d->m_mainTrack.stop();
    for (SoundTrack *track : std::as_const(d->m_soundTracks)) {
        track->stop();
    }
    for (int i = d->m_animatedUpdateTracks.size() - 1; i >= 0; i--) {
        d->m_animatedUpdateTracks[i]->stop();
    }
    hideBalloon();
}

void TourPlayback::seek(double value)
{
    double const offset = qBound(0.0, value, d->m_mainTrack.duration());
    d->m_mainTrack.seek(offset);
    for (SoundTrack *track : std::as_const(d->m_soundTracks)) {
        track->seek(offset);
    }
    for (AnimatedUpdateTrack *track : std::as_const(d->m_animatedUpdateTracks)) {
        track->seek(offset);
    }
}

int TourPlayback::mainTrackSize()
{
    return d->m_mainTrack.size();
}

PlaybackItem *TourPlayback::mainTrackItemAt(int i)
{
    return d->m_mainTrack.at(i);
}

void TourPlayback::updateTracks()
{
    clearTracks();
    double delay = 0;
    for (int i = 0; i < d->m_tour->playlist()->size(); i++) {
        GeoDataTourPrimitive *primitive = d->m_tour->playlist()->primitive(i);
        if (const auto flyTo = geodata_cast<GeoDataFlyTo>(primitive)) {
            d->m_mainTrack.append(new PlaybackFlyToItem(flyTo));
            delay += flyTo->duration();
        } else if (const auto wait = geodata_cast<GeoDataWait>(primitive)) {
            d->m_mainTrack.append(new PlaybackWaitItem(wait));
            delay += wait->duration();
        } else if (const auto tourControl = geodata_cast<GeoDataTourControl>(primitive)) {
            d->m_mainTrack.append(new PlaybackTourControlItem(tourControl));
        } else if (const auto soundCue = geodata_cast<GeoDataSoundCue>(primitive)) {
            auto item = new PlaybackSoundCueItem(soundCue);
            auto track = new SoundTrack(item);
            track->setDelayBeforeTrackStarts(delay);
            d->m_soundTracks.append(track);
        } else if (const auto animatedUpdate = geodata_cast<GeoDataAnimatedUpdate>(primitive)) {
            auto item = new PlaybackAnimatedUpdateItem(animatedUpdate);
            auto track = new AnimatedUpdateTrack(item);
            track->setDelayBeforeTrackStarts(delay + animatedUpdate->delayedStart());
            d->m_animatedUpdateTracks.append(track);
            connect(track, &AnimatedUpdateTrack::balloonHidden, this, &TourPlayback::hideBalloon);
            connect(track, &AnimatedUpdateTrack::balloonShown, this, &TourPlayback::showBalloon);
            connect(track, &AnimatedUpdateTrack::updated, this, &TourPlayback::updated);
            connect(track, &AnimatedUpdateTrack::added, this, &TourPlayback::added);
            connect(track, &AnimatedUpdateTrack::removed, this, &TourPlayback::removed);
        }
    }
    Q_ASSERT(d->m_widget);
    auto lookat = new GeoDataLookAt(d->m_widget->lookAt());
    lookat->setAltitude(lookat->range());
    d->m_mapCenter.setView(lookat);
    auto mapCenterItem = new PlaybackFlyToItem(&d->m_mapCenter);
    PlaybackFlyToItem *before = mapCenterItem;
    for (int i = 0; i < d->m_mainTrack.size(); ++i) {
        auto item = qobject_cast<PlaybackFlyToItem *>(d->m_mainTrack.at(i));
        if (item) {
            item->setBefore(before);
            before = item;
        }
    }
    PlaybackFlyToItem *next = nullptr;
    for (int i = d->m_mainTrack.size() - 1; i >= 0; --i) {
        auto item = qobject_cast<PlaybackFlyToItem *>(d->m_mainTrack.at(i));
        if (item) {
            item->setNext(next);
            next = item;
        }
    }
}

void TourPlayback::clearTracks()
{
    d->m_mainTrack.clear();
    qDeleteAll(d->m_soundTracks);
    qDeleteAll(d->m_animatedUpdateTracks);
    d->m_soundTracks.clear();
    d->m_animatedUpdateTracks.clear();
}

double TourPlayback::duration() const
{
    return d->m_mainTrack.duration();
}

} // namespace Marble

#include "moc_TourPlayback.cpp"
