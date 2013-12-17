#include <QTimer>
#include <QList>

#include "MarbleDebug.h"
#include "MarbleWidget.h"
#include "GeoDataTour.h"
#include "GeoDataTourPrimitive.h"
#include "GeoDataFlyTo.h"
#include "GeoDataLookAt.h"
#include "GeoDataCamera.h"
#include "GeoDataWait.h"
#include "GeoDataSoundCue.h"
#include "GeoDataTypes.h"

#include "TourPlayback.h"

namespace Marble
{

class TimedSoundCue: public QPair<QDateTime, const GeoDataSoundCue*>
{
public:
    bool operator< (const TimedSoundCue &other) const {
        return first < other.first;
    }
};

class TourPlaybackPrivate
{
public:
    TourPlaybackPrivate(TourPlayback *q) :
        m_currentPrimitive(-1),
        m_pause(false),
        q_ptr(q)
    {}

    MarbleWidget *m_widget;
    const GeoDataTour  *m_tour;
    QList<TimedSoundCue> m_cues;
    int m_currentPrimitive;
    bool m_pause;

protected:
    TourPlayback *q_ptr;

public:
    void processNextPrimitive();
    void bounceToCurrentPrimitive();
    void playSoundCue();

private:
    Q_DECLARE_PUBLIC(TourPlayback)
};

TourPlayback::TourPlayback(QObject *parent, GeoDataTour *tour) :
    QObject(parent),
    d_ptr(new TourPlaybackPrivate(this))
{
    Q_D(TourPlayback);
    d->m_tour = tour;
}

TourPlayback::~TourPlayback()
{
    delete d_ptr;
}

bool TourPlayback::isPlaying() const
{
    Q_D(const TourPlayback);
    return !d->m_pause;
}

void TourPlayback::play()
{
    Q_D(TourPlayback);
    d->m_cues.clear();
    d->m_pause = false;
    d->processNextPrimitive();
}

void TourPlayback::pause()
{
    Q_D(TourPlayback);
    d->m_pause = true;
    emit paused();
}

void TourPlayback::stop()
{
    Q_D(TourPlayback);
    d->m_currentPrimitive = -1;
    d->m_pause = true;
    emit stopped();
}

void TourPlaybackPrivate::processNextPrimitive()
{
    Q_Q(TourPlayback);

    if (m_pause)
        return;

    m_currentPrimitive++;
    if (m_currentPrimitive >= m_tour->playlist()->size()) {
        emit q->finished();
        m_currentPrimitive = -1;
        m_cues.clear();
        return;
    }

    const GeoDataTourPrimitive *item =
            m_tour->playlist()->primitive(m_currentPrimitive);

    const GeoDataFlyTo *flyTo = dynamic_cast<const GeoDataFlyTo*>(item);
    if (flyTo) {
        QTimer::singleShot(flyTo->duration()*1000, q, SLOT(bounceToCurrentPrimitive()));
        return;
    }

    const GeoDataWait *wait = dynamic_cast<const GeoDataWait*>(item);
    if (wait) {
        QTimer::singleShot(wait->duration()*1000, q, SLOT(processNextPrimitive()));
        return;
    }

    const GeoDataSoundCue *cue = dynamic_cast<const GeoDataSoundCue*>(item);
    if (cue) {
        TimedSoundCue timed;
        timed.first = QDateTime::currentDateTime().addMSecs(cue->delayedStart()*1000);
        timed.second = cue;

        QList<TimedSoundCue>::iterator iter = m_cues.begin();
        for (; iter != m_cues.end() && *iter < timed; ++iter) {}
        m_cues.insert(iter, timed);

        QTimer::singleShot(cue->delayedStart()*1000, q, SLOT(playSoundCue()));
        processNextPrimitive();
    }
}

void TourPlaybackPrivate::bounceToCurrentPrimitive()
{
    const GeoDataTourPrimitive *item =
            m_tour->playlist()->primitive(m_currentPrimitive);

    const GeoDataFlyTo *flyTo = dynamic_cast<const GeoDataFlyTo*>(item);
    if (flyTo) {
        if (flyTo->view() &&
            flyTo->view()->nodeType() == GeoDataTypes::GeoDataLookAtType) {
            const GeoDataLookAt *lookAt = static_cast<const GeoDataLookAt*>(flyTo->view());
            if (lookAt) {
                GeoDataCoordinates coord = lookAt->coordinates();
                Q_Q(TourPlayback);
                emit q->centerOn(coord);
            }
        } else if (flyTo->view() &&
                   flyTo->view()->nodeType() == GeoDataTypes::GeoDataCameraType) {
            const GeoDataCamera *camera = static_cast<const GeoDataCamera*>(flyTo->view());
            if (camera) {
                GeoDataCoordinates coord = camera->coordinates();
                Q_Q(TourPlayback);
                emit q->centerOn(coord);
            }
        }
    }
    processNextPrimitive();
}

void TourPlaybackPrivate::playSoundCue()
{
    if (m_cues.isEmpty())
        return;

    const GeoDataSoundCue *cue = m_cues.first().second;
    m_cues.removeFirst();

    if (cue) {
        mDebug() << "[TourPlayback]" << cue->href() << "should be going on now";
    }
}

} // namespace Marble

#include "TourPlayback.moc"
