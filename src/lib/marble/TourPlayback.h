//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef TOURPLAYBACK_H
#define TOURPLAYBACK_H

#include <QObject>
#include <QSlider>

#include "marble_export.h"
#include "GeoDataFlyTo.h"
#include "GeoDataWait.h"
#include "GeoDataTourControl.h"
#include "GeoDataSoundCue.h"
#include "GeoDataAnimatedUpdate.h"
#include "SerialTrack.h"
#include "ParallelTrack.h"

namespace Marble
{

class MarbleWidget;

class GeoDataCoordinates;
class GeoDataTour;
class GeoDataTourPrimitive;

class TourPlaybackPrivate;
class SerialTrack;
class ParallelTrack;
class PlaybackItem;
class PlaybackFlyToItem;
class PlaybackWaitItem;
class PlaybackTourControlItem;
class PlaybackSoundCueItem;
class PlaybackAnimatedUpdateItem;

class MARBLE_EXPORT TourPlayback : public QObject
{
    Q_OBJECT
public:
    explicit TourPlayback(QObject *parent);
    ~TourPlayback();

    bool isPlaying() const;

    void setTour(const GeoDataTour *tour);
    void setupProgressBar( QSlider *slider );
    void setMarbleWidget( MarbleWidget *widget );

    void play();
    void pause();
    void stop();
    void seek( double t );

Q_SIGNALS:
    void finished();
    void paused();
    void stopped();
    void centerOn( const GeoDataCoordinates &coordinates );
    void progressChanged( double );

public Q_SLOTS:
    void finishedSlot();
    //void stopPlaying();

private:
    TourPlaybackPrivate * const d;
    SerialTrack* mainTrack();
    friend class TourPlaybackPrivate;
};

} // namespace Marble

#endif // TOURPLAYBACK_H
