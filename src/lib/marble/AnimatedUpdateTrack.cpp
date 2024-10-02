// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "AnimatedUpdateTrack.h"

#include "PlaybackAnimatedUpdateItem.h"

namespace Marble
{

AnimatedUpdateTrack::AnimatedUpdateTrack(PlaybackAnimatedUpdateItem *item)
{
    m_item = item;
    m_progress = 0;
    m_delayBeforeTrackStarts = 0;
    m_paused = true;
    connect(&m_timer, &QTimer::timeout, this, &AnimatedUpdateTrack::playSlot);
    connect(m_item, &PlaybackItem::balloonHidden, this, &AnimatedUpdateTrack::balloonHidden);
    connect(m_item, &PlaybackItem::balloonShown, this, &AnimatedUpdateTrack::balloonShown);
    connect(m_item, &PlaybackItem::updated, this, &AnimatedUpdateTrack::updated);
    connect(m_item, &PlaybackItem::added, this, &AnimatedUpdateTrack::added);
    connect(m_item, &PlaybackItem::removed, this, &AnimatedUpdateTrack::removed);
}

void AnimatedUpdateTrack::setDelayBeforeTrackStarts(double delay)
{
    m_delayBeforeTrackStarts = delay;
    m_timer.setSingleShot(true);
    m_timer.setInterval(m_delayBeforeTrackStarts * 1000);
}

double AnimatedUpdateTrack::delayBeforeTrackStarts() const
{
    return m_delayBeforeTrackStarts;
}

void AnimatedUpdateTrack::play()
{
    m_paused = false;
    m_playTime = QDateTime::currentDateTime();
    if (m_progress <= m_delayBeforeTrackStarts) {
        m_timer.start((m_delayBeforeTrackStarts - m_progress) * 1000);
    } else {
        m_item->play();
    }
}

void AnimatedUpdateTrack::playSlot()
{
    m_item->play();
}

void AnimatedUpdateTrack::pause()
{
    m_paused = true;
    m_pauseTime = QDateTime::currentDateTime();
    m_progress += m_playTime.secsTo(m_pauseTime);
    if (m_timer.isActive()) {
        m_timer.stop();
    } else {
        m_item->pause();
    }
}

void AnimatedUpdateTrack::seek(double offset)
{
    m_timer.stop();
    m_progress = offset;
    m_playTime = QDateTime::currentDateTime().addMSecs(-offset * 1000);

    if (offset <= m_delayBeforeTrackStarts) {
        if (!m_paused) {
            m_pauseTime = QDateTime();
            m_item->stop();
            m_timer.start((m_delayBeforeTrackStarts - m_progress) * 1000);
        } else {
            m_pauseTime = QDateTime::currentDateTime();
            m_item->stop();
        }
    } else {
        if (!m_paused) {
            m_pauseTime = QDateTime();
            m_item->seek(offset - m_delayBeforeTrackStarts);
        } else {
            m_pauseTime = QDateTime::currentDateTime();
            m_item->stop();
            m_item->seek(offset - m_delayBeforeTrackStarts);
        }
    }
}

void AnimatedUpdateTrack::stop()
{
    m_paused = true;
    m_item->stop();
    m_timer.stop();
    m_playTime = QDateTime();
    m_pauseTime = QDateTime();
    m_progress = 0;
}

void AnimatedUpdateTrack::setPaused(bool pause)
{
    m_paused = pause;
}

}

#include "moc_AnimatedUpdateTrack.cpp"
