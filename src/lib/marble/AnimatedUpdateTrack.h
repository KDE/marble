// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef ANIMATEDUPDATETRACK_H
#define ANIMATEDUPDATETRACK_H

#include <QDateTime>
#include <QTimer>

namespace Marble
{

class GeoDataFeature;
class GeoDataContainer;
class GeoDataPlacemark;
class PlaybackAnimatedUpdateItem;

class AnimatedUpdateTrack : public QObject
{
    Q_OBJECT

public:
    explicit AnimatedUpdateTrack(PlaybackAnimatedUpdateItem *item);
    void setDelayBeforeTrackStarts(double delay);
    double delayBeforeTrackStarts() const;
    void play();
    void pause();
    void seek(double position);
    void stop();
    void setPaused(bool);

Q_SIGNALS:
    void balloonHidden();
    void balloonShown(GeoDataPlacemark *);
    void updated(GeoDataFeature *);
    void added(GeoDataContainer *parent, GeoDataFeature *feature, int row);
    void removed(const GeoDataFeature *feature);

public Q_SLOTS:
    void playSlot();

private:
    PlaybackAnimatedUpdateItem *m_item;
    QDateTime m_playTime;
    QDateTime m_pauseTime;
    double m_delayBeforeTrackStarts;
    double m_progress;
    QTimer m_timer;
    bool m_paused;
};
}
#endif
