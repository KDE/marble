//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
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
    explicit PlaybackFlyToItem( const GeoDataFlyTo* flyTo );
    const GeoDataFlyTo* flyTo() const;
    double duration() const;
    void play();
    void pause();
    void seek( double position );
    void stop();
    void center( double t );
    void setBefore(PlaybackFlyToItem* before);
    void setNext(PlaybackFlyToItem* next);
    void setStartCoordinates( const GeoDataCoordinates &coordinates );
    void setFirst( bool isFirst );

private Q_SLOTS:
    void playNext();

private:
    const GeoDataFlyTo* m_flyTo;
    PlaybackFlyToItem* m_before;
    PlaybackFlyToItem* m_next;
    QDateTime m_start;
    QDateTime m_pause;
    bool m_isPlaying;
    bool m_isFirst;
};

}
#endif
