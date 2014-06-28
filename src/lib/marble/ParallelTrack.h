//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef PARALLELTRACK_H
#define PARALLELTRACK_H

#include "PlaybackItem.h"
#include "GeoDataPlacemark.h"

#include <QDateTime>
#include <QTimer>

namespace Marble
{
class ParallelTrack : public QObject
{
    Q_OBJECT

public:
    ParallelTrack( PlaybackItem* item );
    void setDelayBeforeTrackStarts( double delay );
    double delayBeforeTrackStarts() const;
    void play();
    void pause();
    void seek( double position );
    void stop();
    void setPaused( bool );

Q_SIGNALS:
    void balloonHidden();
    void balloonShown( GeoDataPlacemark* );

public Q_SLOTS:
    void playSlot();

private:
    PlaybackItem* m_item;
    QDateTime m_playTime;
    QDateTime m_pauseTime;
    double m_delayBeforeTrackStarts;
    double m_progress;
    QTimer m_timer;
    bool m_paused;
};
}
#endif