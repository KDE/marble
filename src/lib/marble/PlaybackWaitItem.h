//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
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
    explicit PlaybackWaitItem( const GeoDataWait* wait );
    const GeoDataWait* wait() const;
    double duration() const;
    void play();
    void pause();
    void seek( double position );
    void stop();

private Q_SLOTS:
    void playNext();

private:
    const GeoDataWait* m_wait;
    QDateTime m_start;
    QDateTime m_pause;
    bool m_isPlaying;
};
}
#endif
