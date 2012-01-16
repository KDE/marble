//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "RunnerTask.h"

#include "MarbleAbstractRunner.h"
#include "MarbleDebug.h"
#include "MarbleRunnerManager.h"
#include "RunnerPlugin.h"
#include "routing/RouteRequest.h"

#include <QtCore/QTimer>

namespace Marble
{

RunnerTask::RunnerTask( RunnerPlugin* factory, MarbleRunnerManager *manager ) :
    m_factory( factory ),
    m_manager( manager )
{
    // nothing to do
}

void RunnerTask::run()
{
    runTask();

    emit finished( this );
}

RunnerPlugin *RunnerTask::factory()
{
    return m_factory;
}

MarbleRunnerManager *RunnerTask::manager()
{
    return m_manager;
}

SearchTask::SearchTask(RunnerPlugin* factory, MarbleRunnerManager *manager, MarbleModel *model, const QString &searchTerm) :
    RunnerTask( factory, manager ),
    m_model( model ),
    m_searchTerm( searchTerm )
{
    // nothing to do
}

void SearchTask::runTask()
{
    MarbleAbstractRunner *runner = factory()->newRunner();
    connect( runner, SIGNAL( searchFinished( QVector<GeoDataPlacemark*> ) ),
             manager(), SLOT( addSearchResult( QVector<GeoDataPlacemark*> ) ) );
    runner->setModel( m_model );
    runner->search( m_searchTerm );
    runner->deleteLater();
}

ReverseGeocodingTask::ReverseGeocodingTask( RunnerPlugin* factory, MarbleRunnerManager *manager, MarbleModel *model, const GeoDataCoordinates &coordinates ) :
    RunnerTask( factory, manager ),
    m_model( model ),
    m_coordinates( coordinates )
{
    // nothing to do
}

void ReverseGeocodingTask::runTask()
{
    MarbleAbstractRunner *runner = factory()->newRunner();
    connect( runner, SIGNAL( reverseGeocodingFinished( GeoDataCoordinates, GeoDataPlacemark ) ),
             manager(), SLOT( addReverseGeocodingResult( GeoDataCoordinates, GeoDataPlacemark ) ) );
    runner->setModel( m_model );
    runner->reverseGeocoding( m_coordinates );
    runner->deleteLater();
}

RoutingTask::RoutingTask( RunnerPlugin* factory, MarbleRunnerManager *manager, MarbleModel *model, const RouteRequest* routeRequest ) :
    RunnerTask( factory, manager ),
    m_model( model ),
    m_routeRequest( routeRequest )
{
    // nothing to do
}

void RoutingTask::runTask()
{
    MarbleAbstractRunner *runner = factory()->newRunner();
    connect( runner, SIGNAL( routeCalculated( GeoDataDocument* ) ),
             manager(), SLOT( addRoutingResult( GeoDataDocument* ) ) );
    runner->setModel( m_model );
    runner->retrieveRoute( m_routeRequest );
    runner->deleteLater();
}

ParsingTask::ParsingTask( RunnerPlugin *factory, MarbleRunnerManager *manager, const QString& fileName, DocumentRole role ) :
    RunnerTask( factory, manager ),
    m_fileName( fileName ),
    m_role( role )
{
    // nothing to do
}

void ParsingTask::runTask()
{
    MarbleAbstractRunner *runner = factory()->newRunner();
    connect( runner, SIGNAL( parsingFinished( GeoDataDocument*, QString ) ),
             manager(), SLOT( addParsingResult( GeoDataDocument*, QString ) ) );
    runner->parseFile( m_fileName, m_role );
    runner->deleteLater();
}

}

#include "RunnerTask.moc"
