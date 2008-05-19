// Copyright 2007-2008 David Roberts
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


#ifndef EXTDATETIME_H
#define EXTDATETIME_H

#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>

class ExtDateTime : public QObject
{
    Q_OBJECT
	
 public:
    explicit ExtDateTime();
    virtual ~ExtDateTime();

    /**
     * @brief Convert 1BCE to year 0, etc.
     **/
    int year0();

    /**
     * @brief Convert to julian day number. Adapted from http://en.wikipedia.org/wiki/Julian_day#Calculation
     **/
    long toJDN();
    double dayFraction();

    /**
     * @brief Sets the internal date and time with the current one
     **/
    void setNow() {m_datetime = QDateTime::currentDateTime().toUTC();}

    /**
     * @brief Returns the internal date and time
     **/
    QDateTime datetime() {return m_datetime;}

    /**
     * @brief Sets the internal date and time a custom one
     * @param datetime The custom date and time
     **/
    void setDateTime(QDateTime datetime);

    /**
     * @brief Sets the speed of the timer
     * @param speed The new speed (integer)
     **/
    void setSpeed(int speed) {m_speed = speed;}

 private Q_SLOTS:
    void timerTimeout();

 Q_SIGNALS:
    void timeChanged();

 protected:
    int        m_speed;
    QTimer    *m_timer;
    QDateTime  m_datetime;
    int        m_lastmin;
};

#endif
