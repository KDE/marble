#include "TourPlayback.h"

#include <QTimer>
#include <QList>
#include <qurl.h>

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
#include "config-phonon.h"

#ifdef HAVE_PHONON
#include <phonon/MediaObject>
#include <phonon/AudioOutput>
#endif

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
        m_tour(&GeoDataTour::null),
        m_currentPrimitive(-1),
        m_pause(false),
        q_ptr(q)
    {}

    const GeoDataTour  *m_tour;
    QList<TimedSoundCue> m_cues;
    int m_currentPrimitive;
    bool m_pause;
#ifdef HAVE_PHONON
    QList<Phonon::MediaObject*> m_mediaList;
#endif

protected:
    TourPlayback *q_ptr;

public:
    void processNextPrimitive();
    void bounceToCurrentPrimitive();
    void playSoundCue();
    void resumePlaying();
    void stopPlaying();

private:
    Q_DECLARE_PUBLIC(TourPlayback)
};

TourPlayback::TourPlayback(QObject *parent) :
    QObject(parent),
    d_ptr(new TourPlaybackPrivate(this))
{
#ifdef HAVE_PHONON
    connect( this, SIGNAL( finished() ), this, SLOT( stopPlaying() ) );
#endif
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

void TourPlayback::setTour(const GeoDataTour *tour)
{
    Q_D(TourPlayback);
    if (tour) {
        d->m_tour = tour;
    }
    else {
        d->m_tour = &GeoDataTour::null;
    }
}

void TourPlayback::play()
{
    Q_D(TourPlayback);
    d->m_cues.clear();
    d->m_pause = false;
    d->resumePlaying();
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
#ifdef HAVE_PHONON
        Q_Q(TourPlayback);
        Phonon::MediaObject *mediaObject = new Phonon::MediaObject( q );
        Phonon::createPath( mediaObject, new Phonon::AudioOutput( Phonon::MusicCategory, q ) );
        mediaObject->setCurrentSource( QUrl( cue->href() ) );
        mediaObject->play();
        QObject::connect( q, SIGNAL( paused() ), mediaObject, SLOT( pause() ) );
        m_mediaList.append( mediaObject );
#endif
    }
}

void TourPlaybackPrivate::stopPlaying()
{
#ifdef HAVE_PHONON
    QList<Phonon::MediaObject*>::iterator iter = m_mediaList.begin();
    QList<Phonon::MediaObject*>::iterator end = m_mediaList.end();
    for (; iter != end; ++ iter) {
        (*iter)->stop();
    }
    m_mediaList.clear();
#endif // HAVE_PHONON
}

void TourPlaybackPrivate::resumePlaying()
{
#ifdef HAVE_PHONON
    QList<Phonon::MediaObject*>::iterator iter = m_mediaList.begin();
    QList<Phonon::MediaObject*>::iterator end = m_mediaList.end();
    for (; iter != end; ++iter) {
        if ( (*iter)->state() == Phonon::PausedState ) {
            (*iter)->play();
        }
    }
#endif // HAVE_PHONON
}

} // namespace Marble

#include "TourPlayback.moc"
