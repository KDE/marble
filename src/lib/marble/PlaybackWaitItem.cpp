//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "PlaybackWaitItem.h"

#include "GeoDataWait.h"

#include <QTimer>

namespace Marble
{

PlaybackWaitItem::PlaybackWaitItem( const GeoDataWait* wait )
{
    m_wait = wait;
    m_isPlaying = false;
}
const GeoDataWait* PlaybackWaitItem::wait() const
{
    return m_wait;
}
double PlaybackWaitItem::duration() const
{
    return m_wait->duration();
}

void PlaybackWaitItem::play()
{
    if( m_isPlaying ){
        return;
    } else {
        m_isPlaying = true;
        if ( !( m_start.isValid() ) ){
            m_start = QDateTime::currentDateTime();
            Q_ASSERT( m_start.isValid() );
        } else {
            m_start = m_start.addMSecs( m_pause.msecsTo( QDateTime::currentDateTime() ) );
        }
        playNext();
    }
}

void PlaybackWaitItem::playNext()
{
    if( !m_start.isValid() ){
        return;
    }
    double const progress = m_start.msecsTo( QDateTime::currentDateTime() ) / 1000.0;
    Q_ASSERT( progress >= 0.0 );
    double const t = progress / m_wait->duration();
    if( t <= 1 ){
        if( m_isPlaying ){
            emit progressChanged( progress );
            QTimer::singleShot( 20, this, SLOT(playNext()) );
        }
    } else {
        stop();
        emit finished();
    }
}

void PlaybackWaitItem::pause()
{
    m_isPlaying = false;
    m_pause = QDateTime::currentDateTime();
}

void PlaybackWaitItem::seek( double t )
{
    m_start = QDateTime::currentDateTime().addMSecs( -t * m_wait->duration() * 1000 );
    m_pause = QDateTime::currentDateTime();
}

void PlaybackWaitItem::stop()
{
    m_isPlaying = false;
    m_start = QDateTime();
    m_pause = QDateTime();
}

}

#include "moc_PlaybackWaitItem.cpp"
