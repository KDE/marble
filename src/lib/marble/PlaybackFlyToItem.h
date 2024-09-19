// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef PLAYBACKFLYTOITEM_H
#define PLAYBACKFLYTOITEM_H

#include "PlaybackItem.h"

#include <QDateTime>

namespace Marble
{
class GeoDataCoordinates;
class GeoDataFlyTo;

class PlaybackFlyToItem : public PlaybackItem
{
    Q_OBJECT
public:
    explicit PlaybackFlyToItem(const GeoDataFlyTo *flyTo);
    const GeoDataFlyTo *flyTo() const;
    double duration() const override;
    void play() override;
    void pause() override;
    void seek(double position) override;
    void stop() override;
    void center(double t);
    void setBefore(PlaybackFlyToItem *before);
    void setNext(PlaybackFlyToItem *next);
    void setStartCoordinates(const GeoDataCoordinates &coordinates);
    void setFirst(bool isFirst);

private Q_SLOTS:
    void playNext();

private:
    const GeoDataFlyTo *m_flyTo;
    PlaybackFlyToItem *m_before;
    PlaybackFlyToItem *m_next;
    QDateTime m_start;
    QDateTime m_pause;
    bool m_isPlaying;
    bool m_isFirst;
};

}
#endif
