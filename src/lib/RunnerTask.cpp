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
#include "ParseRunnerPlugin.h"
#include "ReverseGeocodingRunnerPlugin.h"
#include "RoutingRunnerPlugin.h"
#include "SearchRunnerPlugin.h"
#include "routing/RouteRequest.h"

#include <QtCore/QTimer>

namespace Marble
{

RunnerTask::RunnerTask( MarbleRunnerManager *manager ) :
    m_manager( manager )
{
    // nothing to do
}

void RunnerTask::run()
{
    runTask();

    emit finished( this );
}

MarbleRunnerManager *RunnerTask::manager()
{
    return m_manager;
}

SearchTask::SearchTask( const SearchRunnerPlugin* factory, MarbleRunnerManager *manager, MarbleModel *model, const QString &searchTerm, const GeoDataLatLonAltBox &preferred ) :
    RunnerTask( manager ),
    m_factory( factory ),
    m_model( model ),
    m_searchTerm( searchTerm ),
    m_preferredBbox( preferred )
{
    // nothing to do
}

void SearchTask::runTask()
{
    MarbleAbstractRunner *runner = m_factory->newRunner();
    connect( runner, SIGNAL( searchFinished( QVector<GeoDataPlacemark*> ) ),
             manager(), SLOT( addSearchResult( QVector<GeoDataPlacemark*> ) ) );
    runner->setModel( m_model );
    runner->search( m_searchTerm, m_preferredBbox );
    runner->deleteLater();
}

ReverseGeocodingTask::ReverseGeocodingTask( const ReverseGeocodingRunnerPlugin* factory, MarbleRunnerManager *manager, MarbleModel *model, const GeoDataCoordinates &coordinates ) :
    RunnerTask( manager ),
    m_factory( factory ),
    m_model( model ),
    m_coordinates( coordinates )
{
    // nothing to do
}

void ReverseGeocodingTask::runTask()
{
    MarbleAbstractRunner *runner = m_factory->newRunner();
    connect( runner, SIGNAL( reverseGeocodingFinished( GeoDataCoordinates, GeoDataPlacemark ) ),
             manager(), SLOT( addReverseGeocodingResult( GeoDataCoordinates, GeoDataPlacemark ) ) );
    runner->setModel( m_model );
    runner->reverseGeocoding( m_coordinates );
    runner->deleteLater();
}

RoutingTask::RoutingTask( RoutingRunnerPlugin* factory, MarbleRunnerManager *manager, MarbleModel *model, const RouteRequest* routeRequest ) :
    RunnerTask( manager ),
    m_factory( factory ),
    m_model( model ),
    m_routeRequest( routeRequest )
{
    // nothing to do
}

void RoutingTask::runTask()
{
    MarbleAbstractRunner *runner = m_factory->newRunner();
    connect( runner, SIGNAL( routeCalculated( GeoDataDocument* ) ),
             manager(), SLOT( addRoutingResult( GeoDataDocument* ) ) );
    runner->setModel( m_model );
    runner->retrieveRoute( m_routeRequest );
    runner->deleteLater();
}

ParsingTask::ParsingTask( const ParseRunnerPlugin *factory, MarbleRunnerManager *manager, const QString& fileName, DocumentRole role ) :
    RunnerTask( manager ),
    m_factory( factory ),
    m_fileName( fileName ),
    m_role( role )
{
    // nothing to do
}

void ParsingTask::runTask()
{
    MarbleAbstractRunner *runner = m_factory->newRunner();
    connect( runner, SIGNAL( parsingFinished( GeoDataDocument*, QString ) ),
             manager(), SLOT( addParsingResult( GeoDataDocument*, QString ) ) );
    runner->parseFile( m_fileName, m_role );
    runner->deleteLater();
}

}

#include "RunnerTask.moc"
