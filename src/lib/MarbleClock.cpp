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
      m_datetime( QDateTime::currentDateTime().toUTC() ),
      m_lastmin( -1 ),
      m_timezoneInSec( 0 )
{

    m_timer = new QTimer( this );
    connect( m_timer, SIGNAL( timeout() ), 
             this,    SLOT( timerTimeout() ) );
    m_timer->start( 1000 );
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
    m_datetime = m_datetime.addSecs( m_speed );

    int  min = m_datetime.time().minute();
    if ( m_lastmin != min ) {
        m_lastmin = min;
        emit timeChanged();
    }
}

void MarbleClock::setDateTime( const QDateTime& datetime )
{
    m_datetime = datetime;
    m_lastmin = m_datetime.time().minute();
    emit timeChanged();
}

QDateTime MarbleClock::dateTime() const
{
    return m_datetime;
}

int MarbleClock::speed() const
{
    return m_speed;
}

void MarbleClock::setSpeed( int speed )
{
    m_speed = speed;
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
