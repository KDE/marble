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

#include "geodata_export.h"

#include <QtCore/QObject>
#include <QtCore/QDateTime>

namespace Marble
{

class MarbleClockPrivate;

class GEODATA_EXPORT MarbleClock : public QObject
{
    Q_OBJECT

 public:
    explicit MarbleClock( QObject* parent = 0 );

    ~MarbleClock();

    /**
     * @brief Determine how much of the current day has elapsed
     * @return A value between 0 and 1 corresponding to the fraction of the day that has elapsed
     */
    qreal dayFraction() const;

 Q_SIGNALS:
    /**
     * @brief the timeChanged signal will be triggered at updateInterval() rate
     * or at most every second.
     **/
    void timeChanged();

    /**
     * @brief Emitted when setUpdateInterval() is called.
     */
    void updateIntervalChanged( int seconds );

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
     * @brief Set the interval at which dateTime() is updated and timeChanged() is emitted.
     * @param seconds The interval in seconds
     * @see updateInterval
     */
    void setUpdateInterval( int seconds );

    /**
     * @brief Returns the interval at which dateTime() is updated and timeChanged() is emitted,
     * The default is 60 seconds.
     * @return The interval in seconds.
     * @see setUpdateInterval
     */
    int updateInterval();

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

 private:
    Q_DISABLE_COPY( MarbleClock )

    Q_PRIVATE_SLOT( d,  void timerTimeout() )

    MarbleClockPrivate* const d;

    friend class MarbleClockPrivate;
};

}
#endif
