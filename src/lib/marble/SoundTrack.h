// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef SOUNDTRACK_H
#define SOUNDTRACK_H

#include <QDateTime>
#include <QTimer>

namespace Marble
{

class PlaybackSoundCueItem;

class SoundTrack : public QObject
{
    Q_OBJECT

public:
    explicit SoundTrack(PlaybackSoundCueItem *item);
    void setDelayBeforeTrackStarts(double delay);
    double delayBeforeTrackStarts() const;
    void play();
    void pause();
    void seek(double position);
    void stop();
    void setPaused(bool);

public Q_SLOTS:
    void playSlot();

private:
    PlaybackSoundCueItem *m_item;
    QDateTime m_playTime;
    QDateTime m_pauseTime;
    double m_delayBeforeTrackStarts;
    double m_progress;
    QTimer m_timer;
    bool m_paused;
};
}
#endif
