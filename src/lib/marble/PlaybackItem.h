// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef PLAYBACKITEM_H
#define PLAYBACKITEM_H

#include <QObject>

namespace Marble
{

class GeoDataContainer;
class GeoDataCoordinates;
class GeoDataFeature;
class GeoDataPlacemark;

class PlaybackItem : public QObject
{
    Q_OBJECT
public:
    ~PlaybackItem() override;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void seek(double position) = 0;
    virtual void stop() = 0;
    virtual double duration() const = 0;

Q_SIGNALS:
    void finished();
    void centerOn(const GeoDataCoordinates &coordinates);
    void paused();
    void progressChanged(double seconds);
    void balloonShown(GeoDataPlacemark *);
    void balloonHidden();
    void updated(GeoDataFeature *);
    void added(GeoDataContainer *parent, GeoDataFeature *feature, int row);
    void removed(const GeoDataFeature *feature);
};
}
#endif
