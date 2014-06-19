//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "TourPlayback.h"

#include <QTimer>
#include <QList>
#include <QSlider>
#include <qurl.h>

#include "MarbleDebug.h"
#include "MarbleWidget.h"
#include "GeoDataTour.h"
#include "GeoDataTourPrimitive.h"
#include "GeoDataFlyTo.h"
#include "GeoDataLookAt.h"
#include "GeoDataCamera.h"
#include "GeoDataWait.h"
#include "GeoDataTourControl.h"
#include "GeoDataSoundCue.h"
#include "GeoDataAnimatedUpdate.h"
#include "GeoDataTypes.h"
#include "PlaybackFlyToItem.h"
#include "PlaybackAnimatedUpdateItem.h"
#include "PlaybackWaitItem.h"
#include "PlaybackTourControlItem.h"
#include "PlaybackSoundCueItem.h"
#include "PlaybackAnimatedUpdateItem.h"

namespace Marble
{

class TourPlaybackPrivate
{
public:
    TourPlaybackPrivate(TourPlayback *q);

    const GeoDataTour  *m_tour;
    bool m_pause;
    SerialTrack *m_mainTrack;
    QList<ParallelTrack*> m_parallelTracks;
    GeoDataCoordinates m_coordinates;
    GeoDataFlyTo m_mapCenter;
    MarbleWidget *m_widget;
    QSlider *m_slider;

protected:
    TourPlayback *q_ptr;

private:
    Q_DECLARE_PUBLIC(TourPlayback)
};

TourPlaybackPrivate::TourPlaybackPrivate(TourPlayback* q) :
    m_tour( &GeoDataTour::null ),
    m_pause( false ),
    m_mainTrack( new SerialTrack),
    q_ptr( q )
{
    // do nothing
}

TourPlayback::TourPlayback(QObject *parent) :
    QObject(parent),
    d(new TourPlaybackPrivate(this))
{
    connect( mainTrack(), SIGNAL( centerOn( GeoDataCoordinates ) ), this, SIGNAL( centerOn( GeoDataCoordinates ) ) );
    connect( mainTrack(), SIGNAL( progressChanged( double ) ), this, SIGNAL( progressChanged( double ) ) );
    connect( mainTrack(), SIGNAL( finished() ), this, SLOT( finishedSlot() ) );
}

TourPlayback::~TourPlayback()
{
    delete d;
}

void TourPlayback::finishedSlot()
{
    foreach( ParallelTrack* track, d->m_parallelTracks) {
        track->stop();
        track->setPaused( false );
    }
}

SerialTrack* TourPlayback::mainTrack()
{
    return d->m_mainTrack;
}

bool TourPlayback::isPlaying() const
{
    return !d->m_pause;
}

void TourPlayback::setMarbleWidget(MarbleWidget* widget)
{
    d->m_widget = widget;
}

void TourPlayback::setupProgressBar( QSlider *slider )
{
    slider->setMaximum( d->m_mainTrack->duration() * 100 );
    d->m_slider = slider;
}

void TourPlayback::setTour(const GeoDataTour *tour)
{
    d->m_mainTrack->clear();
    d->m_parallelTracks.clear();
    if (tour) {
        d->m_tour = tour;
    }
    else {
        d->m_tour = &GeoDataTour::null;
    }
    double delay = 0;
    for( int i = 0; i < d->m_tour->playlist()->size(); i++){
        const GeoDataTourPrimitive* primitive = d->m_tour->playlist()->primitive( i );
        if( primitive->nodeType() == GeoDataTypes::GeoDataFlyToType ){
            const GeoDataFlyTo *flyTo = dynamic_cast<const GeoDataFlyTo*>(primitive);
            d->m_mainTrack->append( new PlaybackFlyToItem( flyTo ) );
            delay += flyTo->duration();
        }
        else if( primitive->nodeType() == GeoDataTypes::GeoDataWaitType ){
            const GeoDataWait *wait = dynamic_cast<const GeoDataWait*>(primitive);

            d->m_mainTrack->append( new PlaybackWaitItem( wait ) );
            delay += wait->duration();
        }
        else if( primitive->nodeType() == GeoDataTypes::GeoDataTourControlType ){
            const GeoDataTourControl *tourControl = dynamic_cast<const GeoDataTourControl*>(primitive);

            d->m_mainTrack->append( new PlaybackTourControlItem( tourControl ) );
        }
        else if( primitive->nodeType() == GeoDataTypes::GeoDataSoundCueType ){
            const GeoDataSoundCue *soundCue = dynamic_cast<const GeoDataSoundCue*>(primitive);
            PlaybackSoundCueItem *item = new PlaybackSoundCueItem( soundCue );
            ParallelTrack *track = new ParallelTrack( item );
            track->setDelayBeforeTrackStarts( delay );
            d->m_parallelTracks.append( track );
        }
        else if( primitive->nodeType() == GeoDataTypes::GeoDataAnimatedUpdateType ){
            const GeoDataAnimatedUpdate *animatedUpdate = dynamic_cast<const GeoDataAnimatedUpdate*>(primitive);
            PlaybackAnimatedUpdateItem *item = new PlaybackAnimatedUpdateItem( animatedUpdate );
            ParallelTrack *track = new ParallelTrack( item );
            track->setDelayBeforeTrackStarts( delay );
            d->m_parallelTracks.append( track );
        }
    }
    Q_ASSERT( d->m_widget );
    d->m_mapCenter.setView( new GeoDataLookAt( d->m_widget->lookAt() ) );
    PlaybackFlyToItem* mapCenterItem = new PlaybackFlyToItem( &d->m_mapCenter );
    PlaybackFlyToItem* before = mapCenterItem;
    for ( int i=0; i<d->m_mainTrack->size(); ++i ) {
        PlaybackFlyToItem* item = qobject_cast<PlaybackFlyToItem*>( d->m_mainTrack->at(i) );
        if ( item ) {
            item->setBefore( before );
            before = item;
        }
    }
    PlaybackFlyToItem* next = 0;
    for ( int i=d->m_mainTrack->size()-1; i>=0; --i ) {
        PlaybackFlyToItem* item = qobject_cast<PlaybackFlyToItem*>( d->m_mainTrack->at(i) );
        if ( item ) {
            item->setNext( next );
            next = item;
        }
    }
}

void TourPlayback::play()
{
    d->m_pause = false;
    GeoDataCoordinates coords = d->m_widget->focusPoint();
    d->m_mapCenter.setView( new GeoDataLookAt( d->m_widget->lookAt() ) );
    mainTrack()->play();
    foreach( ParallelTrack* track, d->m_parallelTracks) {
        track->play();
    }
}

void TourPlayback::pause()
{
    d->m_pause = true;
    mainTrack()->pause();
    foreach( ParallelTrack* track, d->m_parallelTracks) {
        track->pause();
    }
}

void TourPlayback::stop()
{
    d->m_pause = true;
    d->m_mainTrack->stop();
    foreach( ParallelTrack* track, d->m_parallelTracks) {
        track->stop();
    }
}

void TourPlayback::seek( double t )
{
    Q_ASSERT( t >= 0.0 && t <= 1.0 );
    double const offset = t * mainTrack()->duration();
    mainTrack()->seek( offset );
    foreach( ParallelTrack* track, d->m_parallelTracks ){
        track->seek( offset );
    }
}

} // namespace Marble

#include "TourPlayback.moc"
