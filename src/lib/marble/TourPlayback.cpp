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
#include <QtCore/qnamespace.h>

#include "MarbleDebug.h"
#include "MarbleWidget.h"
#include "PopupLayer.h"
#include "GeoDataTour.h"
#include "GeoDataFlyTo.h"
#include "GeoDataLookAt.h"
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
#include "SerialTrack.h"
#include "ParallelTrack.h"

namespace Marble
{

class TourPlaybackPrivate
{
public:
    TourPlaybackPrivate();
    ~TourPlaybackPrivate();

    GeoDataTour *m_tour;
    bool m_pause;
    SerialTrack m_mainTrack;
    QList<ParallelTrack*> m_parallelTracks;
    GeoDataFlyTo m_mapCenter;
    MarbleWidget *m_widget;
};

TourPlaybackPrivate::TourPlaybackPrivate() :
    m_tour( 0 ),
    m_pause( false ),
    m_mainTrack()
{
    // do nothing
}

TourPlaybackPrivate::~TourPlaybackPrivate()
{
    qDeleteAll(m_parallelTracks);
}

TourPlayback::TourPlayback(QObject *parent) :
    QObject(parent),
    d(new TourPlaybackPrivate())
{
    connect( &d->m_mainTrack, SIGNAL( centerOn( GeoDataCoordinates ) ), this, SIGNAL( centerOn( GeoDataCoordinates ) ) );
    connect( &d->m_mainTrack, SIGNAL( progressChanged( double ) ), this, SIGNAL( progressChanged( double ) ) );
    connect( &d->m_mainTrack, SIGNAL( finished() ), this, SLOT( stopTour() ) );
}

TourPlayback::~TourPlayback()
{
    delete d;
}

void TourPlayback::stopTour()
{
    foreach( ParallelTrack* track, d->m_parallelTracks ){
        track->stop();
        track->setPaused( false );
    }
}

void TourPlayback::showBalloon( GeoDataPlacemark* placemark )
{
    GeoDataPoint* point = static_cast<GeoDataPoint*>( placemark->geometry() );
    d->m_widget->popupLayer()->setCoordinates( point->coordinates(), Qt::AlignRight | Qt::AlignVCenter );
    d->m_widget->popupLayer()->setContent( placemark->description() );
    d->m_widget->popupLayer()->setVisible( true );
    d->m_widget->popupLayer()->setSize( QSizeF( 480, 500 ) );
}

void TourPlayback::hideBalloon()
{
    d->m_widget->popupLayer()->setVisible( false );
}

bool TourPlayback::isPlaying() const
{
    return !d->m_pause;
}

void TourPlayback::setMarbleWidget(MarbleWidget* widget)
{
    d->m_widget = widget;
}

void TourPlayback::setTour(GeoDataTour *tour)
{
    d->m_mainTrack.clear();
    qDeleteAll( d->m_parallelTracks );
    d->m_parallelTracks.clear();
    d->m_tour = tour;
    if ( !d->m_tour ) {
        d->m_mainTrack.clear();
        qDeleteAll(d->m_parallelTracks);
        d->m_parallelTracks.clear();
        return;
    }

    double delay = 0;
    for( int i = 0; i < d->m_tour->playlist()->size(); i++){
        GeoDataTourPrimitive* primitive = d->m_tour->playlist()->primitive( i );
        if( primitive->nodeType() == GeoDataTypes::GeoDataFlyToType ){
            const GeoDataFlyTo *flyTo = dynamic_cast<const GeoDataFlyTo*>(primitive);
            d->m_mainTrack.append( new PlaybackFlyToItem( flyTo ) );
            delay += flyTo->duration();
        }
        else if( primitive->nodeType() == GeoDataTypes::GeoDataWaitType ){
            const GeoDataWait *wait = dynamic_cast<const GeoDataWait*>(primitive);

            d->m_mainTrack.append( new PlaybackWaitItem( wait ) );
            delay += wait->duration();
        }
        else if( primitive->nodeType() == GeoDataTypes::GeoDataTourControlType ){
            const GeoDataTourControl *tourControl = dynamic_cast<const GeoDataTourControl*>(primitive);

            d->m_mainTrack.append( new PlaybackTourControlItem( tourControl ) );
        }
        else if( primitive->nodeType() == GeoDataTypes::GeoDataSoundCueType ){
            const GeoDataSoundCue *soundCue = dynamic_cast<const GeoDataSoundCue*>(primitive);
            PlaybackSoundCueItem *item = new PlaybackSoundCueItem( soundCue );
            ParallelTrack *track = new ParallelTrack( item );
            track->setDelayBeforeTrackStarts( delay );
            d->m_parallelTracks.append( track );
        }
        else if( primitive->nodeType() == GeoDataTypes::GeoDataAnimatedUpdateType ){
            GeoDataAnimatedUpdate *animatedUpdate = dynamic_cast<GeoDataAnimatedUpdate*>(primitive);
            PlaybackAnimatedUpdateItem *item = new PlaybackAnimatedUpdateItem( animatedUpdate );
            ParallelTrack *track = new ParallelTrack( item );
            track->setDelayBeforeTrackStarts( delay );
            d->m_parallelTracks.append( track );
            connect( track, SIGNAL( balloonHidden()), this, SLOT( hideBalloon() ) );
            connect( track, SIGNAL( balloonShown( GeoDataPlacemark* ) ), this, SLOT( showBalloon( GeoDataPlacemark* ) ) );
            connect( track, SIGNAL( updated( GeoDataFeature* ) ), this, SIGNAL( updated( GeoDataFeature* ) ) );
            connect( track, SIGNAL(added(GeoDataContainer*,GeoDataFeature*,int)), this, SIGNAL(added(GeoDataContainer*,GeoDataFeature*,int)) );
            connect( track, SIGNAL(removed(const GeoDataFeature*)), this, SIGNAL(removed(const GeoDataFeature*)) );
        }
    }
    Q_ASSERT( d->m_widget );
    GeoDataLookAt* lookat = new GeoDataLookAt( d->m_widget->lookAt() );
    lookat->setAltitude( lookat->range() );
    d->m_mapCenter.setView( lookat );
    PlaybackFlyToItem* mapCenterItem = new PlaybackFlyToItem( &d->m_mapCenter );
    PlaybackFlyToItem* before = mapCenterItem;
    for ( int i=0; i<d->m_mainTrack.size(); ++i ) {
        PlaybackFlyToItem* item = qobject_cast<PlaybackFlyToItem*>( d->m_mainTrack.at(i) );
        if ( item ) {
            item->setBefore( before );
            before = item;
        }
    }
    PlaybackFlyToItem* next = 0;
    for ( int i=d->m_mainTrack.size()-1; i>=0; --i ) {
        PlaybackFlyToItem* item = qobject_cast<PlaybackFlyToItem*>( d->m_mainTrack.at(i) );
        if ( item ) {
            item->setNext( next );
            next = item;
        }
    }
}

void TourPlayback::play()
{
    d->m_pause = false;
    GeoDataLookAt* lookat = new GeoDataLookAt( d->m_widget->lookAt() );
    lookat->setAltitude( lookat->range() );
    d->m_mapCenter.setView( lookat );
    d->m_mainTrack.play();
    foreach( ParallelTrack* track, d->m_parallelTracks) {
        track->play();
    }
}

void TourPlayback::pause()
{
    d->m_pause = true;
    d->m_mainTrack.pause();
    foreach( ParallelTrack* track, d->m_parallelTracks) {
        track->pause();
    }
}

void TourPlayback::stop()
{
    d->m_pause = true;
    d->m_mainTrack.stop();
    foreach( ParallelTrack* track, d->m_parallelTracks) {
        track->stop();
    }
    hideBalloon();
}

void TourPlayback::seek( double value )
{
    double const offset = qBound( 0.0, value, d->m_mainTrack.duration() );
    d->m_mainTrack.seek( offset );
    foreach( ParallelTrack* track, d->m_parallelTracks ){
        track->seek( offset );
    }
}

double TourPlayback::duration() const
{
    return d->m_mainTrack.duration();
}

} // namespace Marble

#include "TourPlayback.moc"
