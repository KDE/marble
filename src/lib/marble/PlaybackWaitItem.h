// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef PLAYBACKWAITITEM_H
#define PLAYBACKWAITITEM_H

#include "PlaybackItem.h"
#include <QDateTime>

namespace Marble
{

class GeoDataWait;

class PlaybackWaitItem : public PlaybackItem
{
    Q_OBJECT
public:
    explicit PlaybackWaitItem(const GeoDataWait *wait);
    const GeoDataWait *wait() const;
    double duration() const override;
    void play() override;
    void pause() override;
    void seek(double position) override;
    void stop() override;

private Q_SLOTS:
    void playNext();

private:
    const GeoDataWait *m_wait;
    QDateTime m_start;
    QDateTime m_pause;
    bool m_isPlaying;
};
}
#endif
