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

#include "marble_export.h"

namespace Marble
{

class MarbleWidget;
class GeoDataCoordinates;
class GeoDataTour;
class GeoDataPlacemark;

class TourPlaybackPrivate;

class MARBLE_EXPORT TourPlayback : public QObject
{
    Q_OBJECT
public:
    explicit TourPlayback(QObject *parent=0);
    ~TourPlayback();

    void setTour(const GeoDataTour *tour);
    void setMarbleWidget( MarbleWidget *widget );

    /** Tour duration in seconds */
    double duration() const;
    bool isPlaying() const;

    void play();
    void pause();
    void stop();

    /**
     * Seek to the given timestamp (in seconds)
     * @param offset Target timestamp in seconds in the range 0..duration()
     */
    void seek( double offset );

Q_SIGNALS:
    void finished();
    void paused();
    void stopped();
    void centerOn( const GeoDataCoordinates &coordinates );
    void progressChanged( double );

private Q_SLOTS:
    void stopTour();
    void hideBalloon();
    void showBalloon( GeoDataPlacemark* );

private:
    TourPlaybackPrivate * const d;
};

} // namespace Marble

#endif // TOURPLAYBACK_H
