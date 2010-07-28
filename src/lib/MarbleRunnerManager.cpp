//
// This file is part of the Marble Desktop Globe.
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
#include "PlacemarkManager.h"
#include "GeoDataPlacemark.h"
#include "PluginManager.h"
#include "RunnerPlugin.h"
#include "RunnerTask.h"
#include "routing/RouteSkeleton.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QThreadPool>

namespace Marble
{

class MarbleRunnerManagerPrivate
{
public:
    QString m_lastSearchTerm;
    QMutex m_modelMutex;
    MarbleMap * m_map;
    MarblePlacemarkModel *m_model;
    int m_searchTasks;
    QVector<GeoDataPlacemark*> m_placemarkContainer;
    QVector<GeoDataDocument*> m_routingResult;
    QList<GeoDataCoordinates> m_reverseGeocodingResults;
    RouteSkeleton* m_routeSkeleton;
    QString m_celestialBodyId;
    bool m_workOffline;
    PluginManager* m_pluginManager;

    MarbleRunnerManagerPrivate( PluginManager* pluginManager );

    ~MarbleRunnerManagerPrivate();

    QList<RunnerPlugin*> plugins( RunnerPlugin::Capability capability );
};
MarbleRunnerManagerPrivate::MarbleRunnerManagerPrivate( PluginManager* pluginManager ) :
        m_map( 0 ),
        m_model( new MarblePlacemarkModel ),
        m_searchTasks( 0 ),
        m_routeSkeleton( 0 ),
        m_celestialBodyId( "earth" ),
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

        if ( !plugin->supportsCelestialBody( m_celestialBodyId ) )
        {
            continue;
        }

        result << plugin;
    }

    return result;
}

MarbleRunnerManager::MarbleRunnerManager( PluginManager* pluginManager, QObject *parent )
    : QObject( parent ), d( new MarbleRunnerManagerPrivate( pluginManager ) )
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
        runner->setMap( d->m_map );
        QThreadPool::globalInstance()->start( new ReverseGeocodingTask( runner, coordinates ) );
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

    d->m_modelMutex.lock();
    d->m_searchTasks = 0;
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
        runner->setMap( d->m_map );
        QThreadPool::globalInstance()->start( new SearchTask( runner, searchTerm ) );
    }
}

void MarbleRunnerManager::addSearchResult( QVector<GeoDataPlacemark*> result )
{
    mDebug() << "Runner reports" << result.size() << " search results";
    if( result.isEmpty() )
        return;

    d->m_modelMutex.lock();
    --d->m_searchTasks;
    int start = d->m_placemarkContainer.size();
    d->m_placemarkContainer << result;
    d->m_model->addPlacemarks( start, result.size() );
    d->m_modelMutex.unlock();
    emit searchResultChanged( d->m_model );

    if ( d->m_searchTasks <= 0 ) {
        emit searchFinished( d->m_lastSearchTerm );
    }
}

void MarbleRunnerManager::setMap( MarbleMap * map )
{
    // TODO: Terminate runners which are making use of the map.
    d->m_map = map;
}

void MarbleRunnerManager::setCelestialBodyId( const QString &celestialBodyId )
{
    d->m_celestialBodyId = celestialBodyId;
}

void MarbleRunnerManager::setWorkOffline( bool offline )
{
    d->m_workOffline = offline;
}

void MarbleRunnerManager::addReverseGeocodingResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark )
{
    if ( !d->m_reverseGeocodingResults.contains( coordinates ) ) {
        d->m_reverseGeocodingResults.push_back( coordinates );
        emit reverseGeocodingFinished( coordinates, placemark );
    }
}

void MarbleRunnerManager::retrieveRoute( RouteSkeleton *skeleton )
{
    d->m_routingResult.clear();
    d->m_routeSkeleton = skeleton;
    QList<RunnerPlugin*> plugins = d->plugins( RunnerPlugin::Routing );
    foreach( RunnerPlugin* plugin, plugins ) {
        MarbleAbstractRunner* runner = plugin->newRunner();
        connect( runner, SIGNAL( routeCalculated( GeoDataDocument* ) ),
                 this, SLOT( addRoutingResult( GeoDataDocument* ) ) );
        runner->setMap( d->m_map );
        QThreadPool::globalInstance()->start( new RoutingTask( runner, skeleton ) );
    }

    if ( plugins.isEmpty() ) {
        mDebug() << "No routing plugins found, cannot retrieve a route";
    }
}

void MarbleRunnerManager::addRoutingResult( GeoDataDocument* route )
{
    if ( route ) {
        emit routeRetrieved( route );
    }
}

}

#include "MarbleRunnerManager.moc"
