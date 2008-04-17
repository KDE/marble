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

    int year0();
    long toJDN();
    double dayFraction();
    void setNow() {m_datetime = QDateTime::currentDateTime().toUTC();}
	
    QDateTime datetime() {return m_datetime;}
	
    void setDateTime(QDateTime datetime);
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
