//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "vehicletracking.h"

#include <QApplication>
#include <QThread>
#include <QTimer>
#include <qmath.h>
#include <QDebug>
#include <QVBoxLayout>

#include <marble/MarbleWidget.h>
#include <marble/MarbleGlobal.h>
#include <marble/GeoDataDocument.h>
#include <marble/GeoDataPlacemark.h>
#include <marble/GeoDataLineString.h>
#include <marble/GeoDataTreeModel.h>
#include <marble/MarbleModel.h>


using namespace Marble;

CarWorker::CarWorker(const GeoDataCoordinates &city, qreal radius, qreal speed) :
    QObject(),
    m_timer(new QTimer(this)),
    m_city(city),
    m_radius(radius),
    m_speed(speed),
    m_alpha(0.0)
{}

void CarWorker::startWork()
{
    m_timer->setInterval(0);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(iterate()));
    m_timer->start();
}

void CarWorker::iterate()
{
    qreal lon = m_city.longitude(GeoDataCoordinates::Degree) + m_radius * qCos(m_alpha * DEG2RAD);
    qreal lat = m_city.latitude(GeoDataCoordinates::Degree) + m_radius * qSin(m_alpha * DEG2RAD);

    GeoDataCoordinates coord(lon, lat, 0.0, GeoDataCoordinates::Degree);
    emit coordinatesChanged(coord);

    m_alpha += m_speed;
}

void CarWorker::finishWork()
{
    m_timer->stop();
}

Window::Window(QWidget *parent) :
    QWidget(parent),
    m_marbleWidget(new MarbleWidget)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_marbleWidget);
    setLayout(layout);

    // Load the OpenStreetMap map
    m_marbleWidget->setMapThemeId(QStringLiteral("earth/openstreetmap/openstreetmap.dgml"));
    m_marbleWidget->setProjection( Mercator );
    setGeometry(80, 60, 1000, 800);
    GeoDataCoordinates Kiev(30.523333, 50.45, 0.0, GeoDataCoordinates::Degree);
    m_marbleWidget->centerOn(Kiev);
    m_marbleWidget->setZoom(2300);

    m_carFirst = new GeoDataPlacemark(QStringLiteral("Bus"));
    m_carSecond = new GeoDataPlacemark(QStringLiteral("Car"));

    GeoDataDocument *document = new GeoDataDocument;

    document->append(m_carFirst);
    document->append(m_carSecond);

    m_marbleWidget->model()->treeModel()->addDocument(document);

    show();
}

void Window::startCars()
{
    GeoDataCoordinates Kiev(30.523333, 50.45, 0.0, GeoDataCoordinates::Degree);

    m_threadFirst = new QThread;
    m_firstWorker = new CarWorker(Kiev, (qreal)0.1, (qreal)0.7);
    m_firstWorker->moveToThread(m_threadFirst);

    connect(m_firstWorker, SIGNAL(coordinatesChanged(GeoDataCoordinates)),
            this, SLOT(setCarCoordinates(GeoDataCoordinates)), Qt::BlockingQueuedConnection);

    m_threadSecond = new QThread;
    m_secondWorker = new CarWorker(Kiev, (qreal)0.2, (qreal)-0.5);
    m_secondWorker->moveToThread(m_threadSecond);

    connect(m_secondWorker, SIGNAL(coordinatesChanged(GeoDataCoordinates)),
            this, SLOT(setCarCoordinates(GeoDataCoordinates)), Qt::BlockingQueuedConnection);

    connect(m_threadFirst, SIGNAL(started()), m_firstWorker, SLOT(startWork()));
    connect(m_threadFirst, SIGNAL(finished()), m_firstWorker, SLOT(finishWork()));

    connect(m_threadSecond, SIGNAL(started()), m_secondWorker, SLOT(startWork()));
    connect(m_threadSecond, SIGNAL(finished()), m_secondWorker, SLOT(finishWork()));

    m_threadFirst->start();
    m_threadSecond->start();
}

void Window::setCarCoordinates(const GeoDataCoordinates &coord)
{
    CarWorker *worker = qobject_cast<CarWorker*>(sender());
    if (worker == m_firstWorker) {
        m_carFirst->setCoordinate(coord);
        m_marbleWidget->model()->treeModel()->updateFeature(m_carFirst);
    } else if (worker == m_secondWorker) {
        m_carSecond->setCoordinate(coord);
        m_marbleWidget->model()->treeModel()->updateFeature(m_carSecond);
    }
}

// Main (start point)
int main(int argc, char** argv)
{
    QApplication app(argc,argv);

    Window window;
    window.startCars();

    return app.exec();
}

#include "moc_vehicletracking.cpp"
