#ifndef TOURPLAYBACK_H
#define TOURPLAYBACK_H

#include <QObject>

#include "marble_export.h"

namespace Marble
{

class GeoDataCoordinates;
class GeoDataTour;
class GeoDataTourPrimitive;

class TourPlaybackPrivate;
class MARBLE_EXPORT TourPlayback : public QObject
{
    Q_OBJECT
public:
    explicit TourPlayback(QObject *parent);
    ~TourPlayback();

    bool isPlaying() const;

    void setTour(const GeoDataTour *tour);

public Q_SLOTS:
    void play();
    void pause();
    void stop();

Q_SIGNALS:
    void finished();
    void paused();
    void stopped();
    void centerOn(const GeoDataCoordinates &coordinates);

protected:
    TourPlaybackPrivate * const d_ptr;

private:
    Q_DECLARE_PRIVATE(TourPlayback)
    Q_PRIVATE_SLOT(d_ptr, void processNextPrimitive())
    Q_PRIVATE_SLOT(d_ptr, void bounceToCurrentPrimitive())
    Q_PRIVATE_SLOT(d_ptr, void playSoundCue())
    Q_PRIVATE_SLOT(d_ptr, void stopPlaying() )

};

} // namespace Marble

#endif // TOURPLAYBACK_H
