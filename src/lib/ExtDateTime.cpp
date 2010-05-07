// Copyright 2007-2008 David Roberts <dvdr18@gmail.com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.


#include "ExtDateTime.h"

#include "MarbleDebug.h"
#include <QtCore/QTimer>

using namespace Marble;

ExtDateTime::ExtDateTime()
    : QObject(),
      m_speed( 1 ),
      m_lastmin( -1 )
{
    setNow();

    m_timer = new QTimer( this );
    connect( m_timer, SIGNAL( timeout() ), 
             this,    SLOT( timerTimeout() ) );
    m_timer->start( 1000 );
}


ExtDateTime::~ExtDateTime()
{
  // FIXME: delete m_timer?
}


int ExtDateTime::year0() const
{
    int year = m_datetime.date().year();
    if ( year < 0 )
        year++;
    return year;
}


long ExtDateTime::toJDN() const
{
    const int EPOCH_G = 32045; // 29 February 4801BCE in gregorian calendar
    const int EPOCH_J = 32083; // 29 February 4801BCE in julian calendar

    int y = year0() + 4800;
    int m = m_datetime.date().month() - 3;

    if ( m_datetime.date().month() <= 2 ) {
        y--;
        m += 12;
    }

    long jdn = m_datetime.date().day() + ((153*m + 2) / 5) + 365*y + y/4;

    if ( jdn >= 2331254 ) {
        // If the date is >= 1582-10-15, then assume gregorian
        // calendar is being used
        jdn += -y/100 + y/400 - EPOCH_G;
    } else {
        // Assume julian calendar is being used.
        jdn -= EPOCH_J;
    }

    return jdn;
}


qreal ExtDateTime::dayFraction() const
{
    qreal f;
    f = m_datetime.time().second();
    f = f/60.0 + m_datetime.time().minute();
    f = f/60.0 + m_datetime.time().hour();
    f = f/24.0;

    return f;
}


void ExtDateTime::timerTimeout()
{
    m_datetime = m_datetime.addSecs( m_speed );

    int  min = m_datetime.time().minute();
    if ( m_lastmin != min ) {
        m_lastmin = min;
        emit timeChanged();
    }
}


void ExtDateTime::setDateTime( const QDateTime& datetime )
{
    m_datetime = datetime;
    m_lastmin = m_datetime.time().minute();
    emit timeChanged();
}

#include "ExtDateTime.moc"
