//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef PLAYBACKITEM_H
#define PLAYBACKITEM_H

#include "GeoDataCoordinates.h"
#include "GeoDataPlacemark.h"
#include <QObject>

namespace Marble
{
class PlaybackItem : public QObject
{
    Q_OBJECT
public:
    virtual ~PlaybackItem();
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void seek( double position ) = 0;
    virtual void stop() = 0;
    virtual double duration() const = 0;

Q_SIGNALS:
    void finished();
    void centerOn( const GeoDataCoordinates &coordinates );
    void paused();
    void progressChanged( double seconds );
    void balloonShown( GeoDataPlacemark* );
    void balloonHidden();
};
}
#endif