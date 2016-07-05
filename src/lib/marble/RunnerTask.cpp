//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
// Copyright 2012,2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#include "RunnerTask.h"

#include "MarbleDebug.h"
#include "ParsingRunner.h"
#include "ParsingRunnerManager.h"
#include "SearchRunner.h"
#include "SearchRunnerManager.h"
#include "ReverseGeocodingRunner.h"
#include "ReverseGeocodingRunnerManager.h"
#include "RoutingRunner.h"
#include "RoutingRunnerManager.h"
#include "routing/RouteRequest.h"

namespace Marble
{

SearchTask::SearchTask( SearchRunner *runner, SearchRunnerManager *manager, const MarbleModel *model, const QString &searchTerm, const GeoDataLatLonBox &preferred ) :
    QObject(),
    m_runner( runner ),
    m_searchTerm( searchTerm ),
    m_preferredBbox( preferred )
{
    connect( m_runner, SIGNAL(searchFinished(QVector<GeoDataPlacemark*>)),
             manager, SLOT(addSearchResult(QVector<GeoDataPlacemark*>)) );
    m_runner->setModel( model );
}

void SearchTask::run()
{
    m_runner->search( m_searchTerm, m_preferredBbox );
    m_runner->deleteLater();

    emit finished( this );
}

ReverseGeocodingTask::ReverseGeocodingTask( ReverseGeocodingRunner *runner, ReverseGeocodingRunnerManager *manager, const MarbleModel *model, const GeoDataCoordinates &coordinates ) :
    QObject(),
    m_runner( runner ),
    m_coordinates( coordinates )
{
    connect( m_runner, SIGNAL(reverseGeocodingFinished(GeoDataCoordinates,GeoDataPlacemark)),
             manager, SLOT(addReverseGeocodingResult(GeoDataCoordinates,GeoDataPlacemark)) );
    m_runner->setModel( model );
}

void ReverseGeocodingTask::run()
{
    m_runner->reverseGeocoding( m_coordinates );
    m_runner->deleteLater();

    emit finished( this );
}

RoutingTask::RoutingTask( RoutingRunner *runner, RoutingRunnerManager *manager, const RouteRequest* routeRequest ) :
    QObject(),
    m_runner( runner ),
    m_routeRequest( routeRequest )
{
    connect( m_runner, SIGNAL(routeCalculated(GeoDataDocument*)),
             manager, SLOT(addRoutingResult(GeoDataDocument*)) );
}

void RoutingTask::run()
{
    m_runner->retrieveRoute( m_routeRequest );
    m_runner->deleteLater();

    emit finished( this );
}

ParsingTask::ParsingTask( ParsingRunner *runner, ParsingRunnerManager *manager, const QString& fileName, DocumentRole role ) :
    QObject(),
    m_runner( runner ),
    m_fileName( fileName ),
    m_role( role ),
    m_manager(manager)
{
    connect(this, SIGNAL(parsed(GeoDataDocument*,QString)), m_manager, SLOT(addParsingResult(GeoDataDocument*,QString)));
}

void ParsingTask::run()
{
    QString error;
    GeoDataDocument* document = m_runner->parseFile( m_fileName, m_role, error );
    emit parsed(document, error);
    m_runner->deleteLater();
    emit finished();
}

}

#include "moc_RunnerTask.cpp"
