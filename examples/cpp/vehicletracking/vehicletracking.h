// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef VEHICLETRACKING
#define VEHICLETRACKING

class QTimer;

#include <marble/GeoDataCoordinates.h>
#include <marble/GeoDataPlacemark.h>

#include <QWidget>
#include <QObject>

namespace Marble {

class MarbleWidget;

// CarWorker Class
class CarWorker : public QObject
{
    Q_OBJECT
public:
    CarWorker(const GeoDataCoordinates& city, qreal radius, qreal speed);

Q_SIGNALS:
    void coordinatesChanged(GeoDataCoordinates coord);

public Q_SLOTS:
    void startWork();
    void finishWork();

private Q_SLOTS:
    void iterate();

private:
    QTimer *m_timer;
    GeoDataCoordinates m_city;
    qreal m_radius;
    qreal m_speed;
    qreal m_alpha;
};

// Window Class
class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = nullptr);
    void startCars();

public Q_SLOTS:
    void setCarCoordinates(const GeoDataCoordinates &coord);

private:
    MarbleWidget *m_marbleWidget;
    CarWorker *m_firstWorker;
    CarWorker *m_secondWorker;
    GeoDataPlacemark *m_carFirst;
    GeoDataPlacemark *m_carSecond;
    QThread *m_threadFirst;
    QThread *m_threadSecond;
};

}

#endif // VEHICLETRACKING
