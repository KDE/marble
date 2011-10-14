//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007-2008 David Roberts <dvdr18@gmail.com>
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#include "MarbleClock.h"
#include <QtCore/QTimer>

#include "MarbleDebug.h"

using namespace Marble;

MarbleClock::MarbleClock()
    : QObject(),
      m_speed( 1 ),
#if QT_VERSION < 0x040700	
      m_datetime( QDateTime::currentDateTime().toUTC() ),
      m_lasttime( QDateTime::currentDateTime().toUTC() ),
#else      
      m_datetime( QDateTime::currentDateTimeUtc() ),
      m_lasttime( QDateTime::currentDateTimeUtc() ),
#endif
      m_timezoneInSec( 0 ),
      m_updateInterval( 60 )
{

    m_timer = new QTimer( this );
    connect( m_timer, SIGNAL( timeout() ), 
             this,    SLOT( timerTimeout() ) );
    timerTimeout();
}


MarbleClock::~MarbleClock()
{
}


qreal MarbleClock::dayFraction() const
{
    qreal f;
    f = m_datetime.time().second();
    f = f/60.0 + m_datetime.time().minute();
    f = f/60.0 + m_datetime.time().hour();
    f = f/24.0;

    return f;
}


void MarbleClock::timerTimeout()
{
    // calculate real period elapsed since last call
#if QT_VERSION < 0x040700
    QDateTime curenttime( QDateTime::currentDateTime().toUTC() );
    int msecdelta = 1000 * m_lasttime.secsTo( curenttime );
#else
    QDateTime curenttime( QDateTime::currentDateTimeUtc() );
    int msecdelta = m_lasttime.msecsTo( curenttime );
#endif
    m_lasttime = curenttime;

    // update m_datetime at m_speed pace
    m_datetime = m_datetime.addMSecs( msecdelta * m_speed );

    // trigger round minute update (at m_speed pace)
    emit timeChanged();

    // sleeptime is the time to sleep until next round minute, at m_speed pace
    int sleeptime = ( m_updateInterval * 1000 - (qreal)(m_datetime.time().msec() + m_datetime.time().second() * 1000 ) ) / m_speed;
    if ( sleeptime < 1000 ) {
        // don't trigger more often than 1s
        sleeptime = 1000;
    }
    m_timer->start( sleeptime );

    //mDebug() << "MarbleClock: will sleep for " << sleeptime;
}

void MarbleClock::setDateTime( const QDateTime& datetime )
{
    m_datetime = datetime;
    timerTimeout();
}

QDateTime MarbleClock::dateTime() const
{
    return m_datetime;
}

void MarbleClock::setUpdateInterval( int seconds )
{
    m_updateInterval = seconds;

    emit updateIntervalChanged( seconds );
}

int MarbleClock::updateInterval()
{
    return m_updateInterval;
}

int MarbleClock::speed() const
{
    return m_speed;
}

void MarbleClock::setSpeed( int speed )
{
    m_speed = speed;
    timerTimeout();
}

int MarbleClock::timezone() const
{
    return m_timezoneInSec;
}

void MarbleClock::setTimezone( int timezoneInSec )
{
    m_timezoneInSec = timezoneInSec;
}

#include "MarbleClock.moc"
