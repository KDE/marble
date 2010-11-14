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

#ifndef MARBLE_MARBLECLOCK_H
#define MARBLE_MARBLECLOCK_H

#include <QtCore/QObject>
#include <QtCore/QDateTime>

class QTimer;

namespace Marble
{

class MarbleClock : public QObject
{
    Q_OBJECT

 public:
    MarbleClock();
    ~MarbleClock();

    qreal dayFraction() const;

 Q_SIGNALS:
    void timeChanged();

public:

    /**
     * @brief Sets the internal date and time a custom one
     * @param datetime The custom date and time
     **/
    void setDateTime( const QDateTime& datetime );


    /**
     * @brief Returns the internal date and time
     **/
    QDateTime dateTime() const;

    /**
     * @brief Sets the speed of the timer which is how fast the marble clock can run relative to actual speed of time.
     * @param speed The new speed (integer)
     **/
    void setSpeed( int speed );

    /**
     * @brief Returns the speed of the timer
     **/
    int speed() const;

    /**
     * @brief Sets the timezone of the clock
     * @param timeInSec The new timezone ( in seconds w.r.t. UTC )
     **/
    void setTimezone( int timeInSec );

    /**
     * @brief Returns the timezone of the clock
     **/
    int timezone() const;

 private Q_SLOTS:
    void timerTimeout();

 protected:
    Q_DISABLE_COPY( MarbleClock )
    int        m_speed;
    QTimer    *m_timer;
    QDateTime  m_datetime;        // stores the UTC time
    int        m_lastmin;
    int        m_timezoneInSec;
};

}
#endif
