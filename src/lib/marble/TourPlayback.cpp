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

#include <QList>
#include <QUrl>
#include <QPointer>

#include "MarbleDebug.h"
#include "MarbleWidget.h"
#include "PopupLayer.h"
#include "GeoDataPoint.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPlaylist.h"
#include "GeoDataTour.h"
#include "GeoDataWait.h"
#include "GeoDataFlyTo.h"
#include "GeoDataLookAt.h"
#include "GeoDataTourControl.h"
#include "GeoDataSoundCue.h"
#include "GeoDataAnimatedUpdate.h"
#include "MarbleModel.h"
#include "GeoDataTreeModel.h"
#include "GeoDataTypes.h"
#include "PlaybackFlyToItem.h"
#include "PlaybackAnimatedUpdateItem.h"
#include "PlaybackWaitItem.h"
#include "PlaybackTourControlItem.h"
#include "PlaybackSoundCueItem.h"
#include "SerialTrack.h"
#include "SoundTrack.h"
#include "AnimatedUpdateTrack.h"

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
    QList<SoundTrack*> m_soundTracks;
    QList<AnimatedUpdateTrack*> m_animatedUpdateTracks;
    GeoDataFlyTo m_mapCenter;
    QPointer<MarbleWidget> m_widget;
    QUrl m_baseUrl;
};

TourPlaybackPrivate::TourPlaybackPrivate() :
    m_tour( 0 ),
    m_pause( false ),
    m_mainTrack(),
    m_widget( 0 )
{
    // do nothing
}

TourPlaybackPrivate::~TourPlaybackPrivate()
{
    qDeleteAll(m_soundTracks);
    qDeleteAll(m_animatedUpdateTracks);
}

TourPlayback::TourPlayback(QObject *parent) :
    QObject(parent),
    d(new TourPlaybackPrivate())
{
    connect( &d->m_mainTrack, SIGNAL(centerOn(GeoDataCoordinates)), this, SLOT(centerOn(GeoDataCoordinates)) );
    connect( &d->m_mainTrack, SIGNAL(progressChanged(double)), this, SIGNAL(progressChanged(double)) );
    connect( &d->m_mainTrack, SIGNAL(finished()), this, SLOT(stopTour()) );
    connect( &d->m_mainTrack, SIGNAL(itemFinished(int)), this, SLOT(handleFinishedItem(int)) );


}

TourPlayback::~TourPlayback()
{
    stop();
    delete d;
}

void TourPlayback::handleFinishedItem( int index )
{
    emit itemFinished( index );
}

void TourPlayback::stopTour()
{
    foreach( SoundTrack* track, d->m_soundTracks ){
        track->stop();
        track->setPaused( false );
    }
    for( int i = d->m_animatedUpdateTracks.size()-1; i >= 0; i-- ){
        d->m_animatedUpdateTracks[ i ]->stop();
        d->m_animatedUpdateTracks[ i ]->setPaused( false );
    }
    emit finished();
}

void TourPlayback::showBalloon( GeoDataPlacemark* placemark )
{
    GeoDataPoint* point = static_cast<GeoDataPoint*>( placemark->geometry() );
    d->m_widget->popupLayer()->setCoordinates( point->coordinates(), Qt::AlignRight | Qt::AlignVCenter );
    d->m_widget->popupLayer()->setContent( placemark->description(), d->m_baseUrl );
    d->m_widget->popupLayer()->setVisible( true );
    d->m_widget->popupLayer()->setSize(QSizeF(500, 520));
}

void TourPlayback::hideBalloon()
{
    if( d->m_widget ){
        d->m_widget->popupLayer()->setVisible( false );
    }
}

bool TourPlayback::isPlaying() const
{
    return !d->m_pause;
}

void TourPlayback::setMarbleWidget(MarbleWidget* widget)
{
    d->m_widget = widget;

    connect( this, SIGNAL(added(GeoDataContainer*,GeoDataFeature*,int)),
                      d->m_widget->model()->treeModel(), SLOT(addFeature(GeoDataContainer*,GeoDataFeature*,int)) );
    connect( this, SIGNAL(removed(const GeoDataFeature*)),
                      d->m_widget->model()->treeModel(), SLOT(removeFeature(const GeoDataFeature*)) );
    connect( this, SIGNAL(updated(GeoDataFeature*)),
                      d->m_widget->model()->treeModel(), SLOT(updateFeature(GeoDataFeature*)) );
}

void TourPlayback::setBaseUrl( const QUrl &baseUrl )
{
    d->m_baseUrl = baseUrl;
}

QUrl TourPlayback::baseUrl() const
{
    return d->m_baseUrl;
}

void TourPlayback::centerOn( const GeoDataCoordinates &coordinates )
{
    if ( d->m_widget ) {
        GeoDataLookAt lookat;
        lookat.setCoordinates( coordinates );
        lookat.setRange( coordinates.altitude() );
        d->m_widget->flyTo( lookat, Instant );
    }
}

void TourPlayback::setTour(GeoDataTour *tour)
{
    d->m_tour = tour;
    if ( !d->m_tour ) {
        clearTracks();
        return;
    }

    updateTracks();
}

void TourPlayback::play()
{
    d->m_pause = false;
    GeoDataLookAt* lookat = new GeoDataLookAt( d->m_widget->lookAt() );
    lookat->setAltitude( lookat->range() );
    d->m_mapCenter.setView( lookat );
    d->m_mainTrack.play();
    foreach( SoundTrack* track, d->m_soundTracks) {
        track->play();
    }
    foreach( AnimatedUpdateTrack* track, d->m_animatedUpdateTracks) {
        track->play();
    }
}

void TourPlayback::pause()
{
    d->m_pause = true;
    d->m_mainTrack.pause();
    foreach( SoundTrack* track, d->m_soundTracks) {
        track->pause();
    }
    foreach( AnimatedUpdateTrack* track, d->m_animatedUpdateTracks) {
        track->pause();
    }
}

void TourPlayback::stop()
{
    d->m_pause = true;
    d->m_mainTrack.stop();
    foreach( SoundTrack* track, d->m_soundTracks) {
        track->stop();
    }
    for( int i = d->m_animatedUpdateTracks.size()-1; i >= 0; i-- ){
        d->m_animatedUpdateTracks[ i ]->stop();
    }
    hideBalloon();
}

void TourPlayback::seek( double value )
{
    double const offset = qBound( 0.0, value, d->m_mainTrack.duration() );
    d->m_mainTrack.seek( offset );
    foreach( SoundTrack* track, d->m_soundTracks ){
        track->seek( offset );
    }
    foreach( AnimatedUpdateTrack* track, d->m_animatedUpdateTracks ){
        track->seek( offset );
    }
}

int TourPlayback::mainTrackSize()
{
    return d->m_mainTrack.size();
}

PlaybackItem* TourPlayback::mainTrackItemAt( int i )
{
    return d->m_mainTrack.at( i );
}

void TourPlayback::updateTracks()
{
    clearTracks();
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
            SoundTrack *track = new SoundTrack( item );
            track->setDelayBeforeTrackStarts( delay );
            d->m_soundTracks.append( track );
        }
        else if( primitive->nodeType() == GeoDataTypes::GeoDataAnimatedUpdateType ){
            GeoDataAnimatedUpdate *animatedUpdate = dynamic_cast<GeoDataAnimatedUpdate*>(primitive);
            PlaybackAnimatedUpdateItem *item = new PlaybackAnimatedUpdateItem( animatedUpdate );
            AnimatedUpdateTrack *track = new AnimatedUpdateTrack( item );
            track->setDelayBeforeTrackStarts( delay + animatedUpdate->delayedStart() );
            d->m_animatedUpdateTracks.append( track );
            connect( track, SIGNAL(balloonHidden()), this, SLOT(hideBalloon()) );
            connect( track, SIGNAL(balloonShown(GeoDataPlacemark*)), this, SLOT(showBalloon(GeoDataPlacemark*)) );
            connect( track, SIGNAL(updated(GeoDataFeature*)), this, SIGNAL(updated(GeoDataFeature*)) );
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

void TourPlayback::clearTracks()
{
    d->m_mainTrack.clear();
    qDeleteAll(d->m_soundTracks);
    qDeleteAll(d->m_animatedUpdateTracks);
    d->m_soundTracks.clear();
    d->m_animatedUpdateTracks.clear();
}

double TourPlayback::duration() const
{
    return d->m_mainTrack.duration();
}

} // namespace Marble

#include "moc_TourPlayback.cpp"
