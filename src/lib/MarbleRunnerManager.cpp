//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#include "MarbleRunnerManager.h"

#include "MarblePlacemarkModel.h"
#include "MarbleDebug.h"
#include "PlacemarkManager.h"
#include "GeoDataPlacemark.h"

#include "HostipRunner.h"
#include "LatLonRunner.h"
#include "OnfRunner.h"
#include "OsmNominatimRunner.h"
#include "LocalDatabaseRunner.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>

namespace Marble
{

MarbleRunnerManager::MarbleRunnerManager( QObject *parent )
    : QObject(parent),
      m_map(0),
      m_activeRunners(0),
      m_model(new MarblePlacemarkModel),
      m_celestialBodyId("earth"),
      m_workOffline(false)
{
    m_model->setPlacemarkContainer(&m_placemarkContainer);
    qRegisterMetaType<QVector<GeoDataPlacemark*> >("QVector<GeoDataPlacemark*>");
}

MarbleRunnerManager::~MarbleRunnerManager()
{
    foreach(MarbleAbstractRunner* runner, m_runners)
    {
        runner->quit();
        runner->wait();
        m_runners.removeOne(runner);
        delete runner;
    }

    delete m_model;
}

void MarbleRunnerManager::newText(const QString& text)
{
    if (text == m_lastString) {
      emit searchFinished(text);
      emit modelChanged( m_model );
      return;
    }

    m_lastString = text;

    m_modelMutex.lock();
    m_model->removePlacemarks("MarbleRunnerManager", 0, m_placemarkContainer.size());
    qDeleteAll(m_placemarkContainer);
    m_placemarkContainer.clear();
    m_modelMutex.unlock();
    emit modelChanged( m_model );

    LatLonRunner* llrunner = new LatLonRunner;
    m_runners << llrunner;
    connect( llrunner, SIGNAL( runnerFinished( MarbleAbstractRunner*, QVector<GeoDataPlacemark*> ) ),
             this,     SLOT( slotRunnerFinished( MarbleAbstractRunner*, QVector<GeoDataPlacemark*> ) ));
    llrunner->parse(text);

    if (m_celestialBodyId == "earth" && !m_workOffline) {
        LocalDatabaseRunner * localDatabaseRunner = new LocalDatabaseRunner;
        m_runners << localDatabaseRunner;
        connect( localDatabaseRunner, SIGNAL( runnerFinished( MarbleAbstractRunner*, QVector<GeoDataPlacemark*> ) ),
                this,      SLOT( slotRunnerFinished( MarbleAbstractRunner*, QVector<GeoDataPlacemark*> ) ));
        localDatabaseRunner->parse(text);
        localDatabaseRunner->setMap(m_map);
        localDatabaseRunner->start();

        OnfRunner* onfrunner = new OnfRunner;
        m_runners << onfrunner;
        connect( onfrunner, SIGNAL( runnerFinished( MarbleAbstractRunner*, QVector<GeoDataPlacemark*> ) ),
                 this,      SLOT( slotRunnerFinished( MarbleAbstractRunner*, QVector<GeoDataPlacemark*> ) ));
        onfrunner->parse(text);
        onfrunner->start();

        OsmNominatimRunner* nominatim = new OsmNominatimRunner;
        m_runners << nominatim;
        connect( nominatim, SIGNAL( runnerFinished( MarbleAbstractRunner*, QVector<GeoDataPlacemark*> ) ),
                 this,      SLOT( slotRunnerFinished( MarbleAbstractRunner*, QVector<GeoDataPlacemark*> ) ));
        nominatim->parse(text);
        nominatim->start();

        HostipRunner* iprunner = new HostipRunner;
        m_runners << iprunner;
        connect( iprunner, SIGNAL( runnerFinished( MarbleAbstractRunner*, QVector<GeoDataPlacemark*> ) ),
                 this,      SLOT( slotRunnerFinished( MarbleAbstractRunner*, QVector<GeoDataPlacemark*> ) ));
        iprunner->parse(text);
        iprunner->start();
    }

    llrunner->start();
}

void MarbleRunnerManager::slotRunnerFinished( MarbleAbstractRunner* runner, QVector<GeoDataPlacemark*> result )
{
    m_runners.removeOne(runner);
    if ( runner ) {
        runner->deleteLater();
    }
    mDebug() << "Runner finished, active runners: " << m_runners.size();
    mDebug() << "Runner reports" << result.size() << "results";
    if( result.isEmpty() )
        return;

    m_modelMutex.lock();
    int start = m_placemarkContainer.size();
    m_placemarkContainer << result;
    m_model->addPlacemarks( start, result.size() );
    m_modelMutex.unlock();
    emit modelChanged( m_model );

    if (m_runners.size() == 0) {
        emit searchFinished(m_lastString);
    }
}

void MarbleRunnerManager::setMap(MarbleMap * map)
{
    // TODO: Terminate runners which are making use of the map.
    m_map = map;
}

void MarbleRunnerManager::setCelestialBodyId(const QString &celestialBodyId)
{
    m_celestialBodyId = celestialBodyId;
}

void MarbleRunnerManager::setWorkOffline( bool offline )
{
    m_workOffline = offline;
}

}

#include "MarbleRunnerManager.moc"
