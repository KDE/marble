//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "SoundTrack.h"

#include "PlaybackSoundCueItem.h"

namespace Marble
{

SoundTrack::SoundTrack( PlaybackSoundCueItem* item )
{
    m_item = item;
    m_progress = 0;
    m_delayBeforeTrackStarts = 0;
    m_paused = true;
    connect( &m_timer, SIGNAL(timeout()), this, SLOT(playSlot()) );
}

void SoundTrack::setDelayBeforeTrackStarts( double delay )
{
    m_delayBeforeTrackStarts = delay;
    m_timer.setSingleShot( true );
    m_timer.setInterval( m_delayBeforeTrackStarts * 1000 );
}

double SoundTrack::delayBeforeTrackStarts() const
{
    return m_delayBeforeTrackStarts;
}

void SoundTrack::play()
{
    m_paused = false;
    m_playTime = QDateTime::currentDateTime();
    if( m_progress <= m_delayBeforeTrackStarts ){
        m_timer.start( ( m_delayBeforeTrackStarts - m_progress ) * 1000 );
    } else {
        m_item->play();
    }
}

void SoundTrack::playSlot()
{
    m_item->play();
}

void SoundTrack::pause()
{
    m_paused = true;
    m_pauseTime = QDateTime::currentDateTime();
    m_progress += m_playTime.msecsTo( m_pauseTime );
    if( m_timer.isActive() ){
        m_timer.stop();
    } else {
        m_item->pause();
    }
}

void SoundTrack::seek( double offset )
{
    m_timer.stop();
    m_progress = offset;
    m_playTime = QDateTime::currentDateTime().addMSecs( -offset * 1000 );

    if( offset <= m_delayBeforeTrackStarts ){
        if( !m_paused ){
            m_pauseTime = QDateTime();
            m_item->stop();
            m_timer.start( ( m_delayBeforeTrackStarts - m_progress ) * 1000 );
        } else {
            m_pauseTime = QDateTime::currentDateTime();
        }
    } else {
        if( !m_paused ){
            m_pauseTime = QDateTime();
            m_item->seek( offset - m_delayBeforeTrackStarts );
        } else {
            m_pauseTime = QDateTime::currentDateTime();
            m_item->seek( offset - m_delayBeforeTrackStarts );
        }
    }
}

void SoundTrack::stop()
{
    m_paused = true;
    m_item->stop();
    m_timer.stop();
    m_playTime = QDateTime();
    m_pauseTime = QDateTime();
    m_progress = 0;
}

void SoundTrack::setPaused( bool pause )
{
    m_paused = pause;
}

}

#include "moc_SoundTrack.cpp"
