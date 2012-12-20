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

SearchTask::SearchTask( MarbleAbstractRunner *runner, MarbleRunnerManager *manager, MarbleModel *model, const QString &searchTerm, const GeoDataLatLonAltBox &preferred ) :
    RunnerTask( manager ),
    m_runner( runner ),
    m_searchTerm( searchTerm ),
    m_preferredBbox( preferred )
{
    connect( m_runner, SIGNAL( searchFinished( QVector<GeoDataPlacemark*> ) ),
             manager, SLOT( addSearchResult( QVector<GeoDataPlacemark*> ) ) );
    m_runner->setModel( model );
}

void SearchTask::runTask()
{
    m_runner->search( m_searchTerm, m_preferredBbox );
    m_runner->deleteLater();
}

ReverseGeocodingTask::ReverseGeocodingTask( MarbleAbstractRunner *runner, MarbleRunnerManager *manager, MarbleModel *model, const GeoDataCoordinates &coordinates ) :
    RunnerTask( manager ),
    m_runner( runner ),
    m_coordinates( coordinates )
{
    connect( m_runner, SIGNAL( reverseGeocodingFinished( GeoDataCoordinates, GeoDataPlacemark ) ),
             manager, SLOT( addReverseGeocodingResult( GeoDataCoordinates, GeoDataPlacemark ) ) );
    m_runner->setModel( model );
}

void ReverseGeocodingTask::runTask()
{
    m_runner->reverseGeocoding( m_coordinates );
    m_runner->deleteLater();
}

RoutingTask::RoutingTask( MarbleAbstractRunner *runner, MarbleRunnerManager *manager, MarbleModel *model, const RouteRequest* routeRequest ) :
    RunnerTask( manager ),
    m_runner( runner ),
    m_routeRequest( routeRequest )
{
    connect( m_runner, SIGNAL( routeCalculated( GeoDataDocument* ) ),
             manager, SLOT( addRoutingResult( GeoDataDocument* ) ) );
    m_runner->setModel( model );
}

void RoutingTask::runTask()
{
    m_runner->retrieveRoute( m_routeRequest );
    m_runner->deleteLater();
}

ParsingTask::ParsingTask( MarbleAbstractRunner *runner, MarbleRunnerManager *manager, const QString& fileName, DocumentRole role ) :
    RunnerTask( manager ),
    m_runner( runner ),
    m_fileName( fileName ),
    m_role( role )
{
    connect( m_runner, SIGNAL( parsingFinished( GeoDataDocument*, QString ) ),
             manager, SLOT( addParsingResult( GeoDataDocument*, QString ) ) );
}

void ParsingTask::runTask()
{
    m_runner->parseFile( m_fileName, m_role );
    m_runner->deleteLater();
}

}

#include "RunnerTask.moc"
