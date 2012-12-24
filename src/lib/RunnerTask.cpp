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

SearchTask::SearchTask( MarbleAbstractRunner *runner, MarbleRunnerManager *manager, MarbleModel *model, const QString &searchTerm, const GeoDataLatLonAltBox &preferred ) :
    QObject(),
    m_runner( runner ),
    m_searchTerm( searchTerm ),
    m_preferredBbox( preferred )
{
    connect( m_runner, SIGNAL( searchFinished( QVector<GeoDataPlacemark*> ) ),
             manager, SLOT( addSearchResult( QVector<GeoDataPlacemark*> ) ) );
    m_runner->setModel( model );
}

void SearchTask::run()
{
    m_runner->search( m_searchTerm, m_preferredBbox );
    m_runner->deleteLater();

    emit finished( this );
}

ReverseGeocodingTask::ReverseGeocodingTask( MarbleAbstractRunner *runner, MarbleRunnerManager *manager, MarbleModel *model, const GeoDataCoordinates &coordinates ) :
    QObject(),
    m_runner( runner ),
    m_coordinates( coordinates )
{
    connect( m_runner, SIGNAL( reverseGeocodingFinished( GeoDataCoordinates, GeoDataPlacemark ) ),
             manager, SLOT( addReverseGeocodingResult( GeoDataCoordinates, GeoDataPlacemark ) ) );
    m_runner->setModel( model );
}

void ReverseGeocodingTask::run()
{
    m_runner->reverseGeocoding( m_coordinates );
    m_runner->deleteLater();

    emit finished( this );
}

RoutingTask::RoutingTask( MarbleAbstractRunner *runner, MarbleRunnerManager *manager, MarbleModel *model, const RouteRequest* routeRequest ) :
    QObject(),
    m_runner( runner ),
    m_routeRequest( routeRequest )
{
    connect( m_runner, SIGNAL( routeCalculated( GeoDataDocument* ) ),
             manager, SLOT( addRoutingResult( GeoDataDocument* ) ) );
    m_runner->setModel( model );
}

void RoutingTask::run()
{
    m_runner->retrieveRoute( m_routeRequest );
    m_runner->deleteLater();

    emit finished( this );
}

ParsingTask::ParsingTask( MarbleAbstractRunner *runner, MarbleRunnerManager *manager, const QString& fileName, DocumentRole role ) :
    QObject(),
    m_runner( runner ),
    m_fileName( fileName ),
    m_role( role )
{
    connect( m_runner, SIGNAL( parsingFinished( GeoDataDocument*, QString ) ),
             manager, SLOT( addParsingResult( GeoDataDocument*, QString ) ) );
}

void ParsingTask::run()
{
    m_runner->parseFile( m_fileName, m_role );
    m_runner->deleteLater();

    emit finished( this );
}

}

#include "RunnerTask.moc"
