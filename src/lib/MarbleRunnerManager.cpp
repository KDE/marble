//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>

#include "MarbleRunnerManager.h"

#include "MarblePlacemarkModel.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "Planet.h"
#include "GeoDataPlacemark.h"
#include "PluginManager.h"
#include "RunnerPlugin.h"
#include "RunnerTask.h"
#include "routing/RouteRequest.h"
#include "routing/RoutingProfilesModel.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QThreadPool>
#include <QtCore/QTimer>

namespace Marble
{

class MarbleModel;

class MarbleRunnerManagerPrivate
{
public:
    MarbleRunnerManager* q;
    QString m_lastSearchTerm;
    QMutex m_modelMutex;
    MarbleModel * m_marbleModel;
    MarblePlacemarkModel *m_model;
    QVector<GeoDataPlacemark*> m_placemarkContainer;
    QVector<GeoDataDocument*> m_routingResult;
    QList<GeoDataCoordinates> m_reverseGeocodingResults;
    RouteRequest* m_routeRequest;
    bool m_workOffline;
    PluginManager* m_pluginManager;

    MarbleRunnerManagerPrivate( MarbleRunnerManager* parent, PluginManager* pluginManager );

    ~MarbleRunnerManagerPrivate();

    QList<RunnerPlugin*> plugins( RunnerPlugin::Capability capability );

    QList<RunnerTask*> m_searchTasks;
    QList<RunnerTask*> m_routingTasks;

    void cleanupSearchTask( RunnerTask* task );

    void cleanupRoutingTask( RunnerTask* task );
};

MarbleRunnerManagerPrivate::MarbleRunnerManagerPrivate( MarbleRunnerManager* parent, PluginManager* pluginManager ) :
        q( parent ),
        m_marbleModel( 0 ),
        m_model( new MarblePlacemarkModel ),
        m_routeRequest( 0 ),
        m_workOffline( false ),
        m_pluginManager( pluginManager )
{
    m_model->setPlacemarkContainer( &m_placemarkContainer );
    qRegisterMetaType<GeoDataPlacemark>( "GeoDataPlacemark" );
    qRegisterMetaType<GeoDataCoordinates>( "GeoDataCoordinates" );
    qRegisterMetaType<QVector<GeoDataPlacemark*> >( "QVector<GeoDataPlacemark*>" );
}

MarbleRunnerManagerPrivate::~MarbleRunnerManagerPrivate()
{
    delete m_model;
}

QList<RunnerPlugin*> MarbleRunnerManagerPrivate::plugins( RunnerPlugin::Capability capability )
{
    QList<RunnerPlugin*> result;
    QList<RunnerPlugin*> plugins = m_pluginManager->runnerPlugins();
    foreach( RunnerPlugin* plugin, plugins ) {
        if ( !plugin->supports( capability ) ) {
            continue;
        }

        if ( ( m_workOffline && !plugin->canWorkOffline() ) ) {
            continue;
        }

        if ( !plugin->canWork( capability ) ) {
            continue;
        }

        if ( m_marbleModel && !plugin->supportsCelestialBody( m_marbleModel->planet()->id() ) )
        {
            continue;
        }

        result << plugin;
    }

    return result;
}

void MarbleRunnerManagerPrivate::cleanupSearchTask( RunnerTask* task )
{
    m_searchTasks.removeAll( task );

    if ( m_searchTasks.isEmpty() ) {
        emit q->searchFinished( m_lastSearchTerm );
    }
}

void MarbleRunnerManagerPrivate::cleanupRoutingTask( RunnerTask* task )
{
    m_routingTasks.removeAll( task );

    if ( m_routingTasks.isEmpty() && m_routingResult.isEmpty() ) {
        emit q->routeRetrieved( 0 );
    }
}

MarbleRunnerManager::MarbleRunnerManager( PluginManager* pluginManager, QObject *parent )
    : QObject( parent ), d( new MarbleRunnerManagerPrivate( this, pluginManager ) )
{
    // nothing to do
}

MarbleRunnerManager::~MarbleRunnerManager()
{
    delete d;
}

void MarbleRunnerManager::reverseGeocoding( const GeoDataCoordinates &coordinates )
{
    d->m_reverseGeocodingResults.removeAll( coordinates );
    QList<RunnerPlugin*> plugins = d->plugins( RunnerPlugin::ReverseGeocoding );
    foreach( RunnerPlugin* plugin, plugins ) {
        MarbleAbstractRunner* runner = plugin->newRunner();
        connect( runner, SIGNAL( reverseGeocodingFinished( GeoDataCoordinates, GeoDataPlacemark ) ),
                 this, SLOT( addReverseGeocodingResult( GeoDataCoordinates, GeoDataPlacemark ) ) );
        runner->setModel( d->m_marbleModel );
        QThreadPool::globalInstance()->start( new ReverseGeocodingTask( runner, coordinates ) );
    }

    if ( plugins.isEmpty() ) {
        emit reverseGeocodingFinished( coordinates, GeoDataPlacemark() );
    }
}

void MarbleRunnerManager::findPlacemarks( const QString &searchTerm )
{
    if ( searchTerm == d->m_lastSearchTerm ) {
      emit searchFinished( searchTerm );
      emit searchResultChanged( d->m_model );
      return;
    }

    d->m_lastSearchTerm = searchTerm;

    d->m_searchTasks.clear();

    d->m_modelMutex.lock();
    d->m_model->removePlacemarks( "MarbleRunnerManager", 0, d->m_placemarkContainer.size() );
    qDeleteAll( d->m_placemarkContainer );
    d->m_placemarkContainer.clear();
    d->m_modelMutex.unlock();
    emit searchResultChanged( d->m_model );

    QList<RunnerPlugin*> plugins = d->plugins( RunnerPlugin::Search );
    foreach( RunnerPlugin* plugin, plugins ) {
        MarbleAbstractRunner* runner = plugin->newRunner();
        connect( runner, SIGNAL( searchFinished( QVector<GeoDataPlacemark*> ) ),
                 this, SLOT( addSearchResult( QVector<GeoDataPlacemark*> ) ) );
        runner->setModel( d->m_marbleModel );
        SearchTask* task = new SearchTask( runner, searchTerm );
        connect( task, SIGNAL( finished( RunnerTask* ) ), this, SLOT( cleanupSearchTask( RunnerTask* ) ) );
        d->m_searchTasks << task;
        QThreadPool::globalInstance()->start( task );
    }
}

void MarbleRunnerManager::addSearchResult( QVector<GeoDataPlacemark*> result )
{
    mDebug() << "Runner reports" << result.size() << " search results";
    if( result.isEmpty() )
        return;

    d->m_modelMutex.lock();
    int start = d->m_placemarkContainer.size();
    d->m_placemarkContainer << result;
    d->m_model->addPlacemarks( start, result.size() );
    d->m_modelMutex.unlock();
    emit searchResultChanged( d->m_model );
}

void MarbleRunnerManager::setModel( MarbleModel * model )
{
    // TODO: Terminate runners which are making use of the map.
    d->m_marbleModel = model;
}

void MarbleRunnerManager::setWorkOffline( bool offline )
{
    d->m_workOffline = offline;
}

void MarbleRunnerManager::addReverseGeocodingResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark )
{
    if ( !d->m_reverseGeocodingResults.contains( coordinates ) && !placemark.address().isEmpty() ) {
        d->m_reverseGeocodingResults.push_back( coordinates );
        emit reverseGeocodingFinished( coordinates, placemark );
    }
}

void MarbleRunnerManager::retrieveRoute( RouteRequest *request )
{
    RoutingProfile profile = request->routingProfile();

    d->m_routingTasks.clear();
    d->m_routingResult.clear();

    d->m_routeRequest = request;
    QList<RunnerPlugin*> plugins = d->plugins( RunnerPlugin::Routing );
    bool started = false;
    foreach( RunnerPlugin* plugin, plugins ) {
        if ( !profile.pluginSettings().contains( plugin->nameId() ) ) {
            continue;
        }

        started = true;
        MarbleAbstractRunner* runner = plugin->newRunner();
        connect( runner, SIGNAL( routeCalculated( GeoDataDocument* ) ),
                 this, SLOT( addRoutingResult( GeoDataDocument* ) ) );
        runner->setModel( d->m_marbleModel );
        RoutingTask* task = new RoutingTask( runner, request );
        d->m_routingTasks << task;
        connect( task, SIGNAL( finished( RunnerTask* ) ), this, SLOT( cleanupRoutingTask( RunnerTask* ) ) );
        QThreadPool::globalInstance()->start( task );
    }

    if ( !started ) {
        mDebug() << "No routing plugins found, cannot retrieve a route";
        d->cleanupRoutingTask( 0 );
    }
}

void MarbleRunnerManager::addRoutingResult( GeoDataDocument* route )
{
    if ( route ) {
        d->m_routingResult.push_back( route );
        emit routeRetrieved( route );
    }
}

}

#include "MarbleRunnerManager.moc"
