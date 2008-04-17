// (c) 2007-2008 David Roberts

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
