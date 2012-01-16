//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "MarbleRunnerManager.h"

#include "MarblePlacemarkModel.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "Planet.h"
#include "GeoDataDocument.h"
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
    MarblePlacemarkModel *m_model;
    QVector<GeoDataPlacemark*> m_placemarkContainer;
    QList<GeoDataCoordinates> m_reverseGeocodingResults;
    QString m_reverseGeocodingResult;
    QVector<GeoDataDocument*> m_routingResult;
    GeoDataDocument* m_fileResult;
    MarbleModel * m_marbleModel;
    const PluginManager* m_pluginManager;

    MarbleRunnerManagerPrivate( MarbleRunnerManager* parent, const PluginManager* pluginManager );

    ~MarbleRunnerManagerPrivate();

    QList<RunnerPlugin*> plugins( RunnerPlugin::Capability capability );

    QList<RunnerTask*> m_searchTasks;
    QList<RunnerTask*> m_reverseTasks;
    QList<RunnerTask*> m_routingTasks;
    QList<RunnerTask*> m_parsingTasks;
    int m_watchdogTimer;

    void addSearchResult( QVector<GeoDataPlacemark*> result );
    void addReverseGeocodingResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark );
    void addRoutingResult( GeoDataDocument* route );
    void addParsingResult( GeoDataDocument* document, const QString& error = QString() );

    void cleanupSearchTask( RunnerTask* task );
    void cleanupReverseGeocodingTask( RunnerTask* task );
    void cleanupRoutingTask( RunnerTask* task );
    void cleanupParsingTask( RunnerTask* task );

};

MarbleRunnerManagerPrivate::MarbleRunnerManagerPrivate( MarbleRunnerManager* parent, const PluginManager* pluginManager ) :
        q( parent ),
        m_model( new MarblePlacemarkModel( parent ) ),
        m_fileResult( 0 ),
        m_marbleModel( 0 ),
        m_pluginManager( pluginManager ),
        m_watchdogTimer( 30000 )
{
    m_model->setPlacemarkContainer( &m_placemarkContainer );
    qRegisterMetaType<GeoDataDocument*>( "GeoDataDocument*" );
    qRegisterMetaType<GeoDataPlacemark>( "GeoDataPlacemark" );
    qRegisterMetaType<GeoDataCoordinates>( "GeoDataCoordinates" );
    qRegisterMetaType<QVector<GeoDataPlacemark*> >( "QVector<GeoDataPlacemark*>" );
}

MarbleRunnerManagerPrivate::~MarbleRunnerManagerPrivate()
{
    // nothing to do
}

QList<RunnerPlugin*> MarbleRunnerManagerPrivate::plugins( RunnerPlugin::Capability capability )
{
    QList<RunnerPlugin*> result;
    QList<RunnerPlugin*> plugins = m_pluginManager->runnerPlugins();
    foreach( RunnerPlugin* plugin, plugins ) {
        if ( !plugin->supports( capability ) ) {
            continue;
        }

        if ( ( m_marbleModel && m_marbleModel->workOffline() && !plugin->canWorkOffline() ) ) {
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
    mDebug() << "removing search task" << m_searchTasks.size() << (long)task;
    if ( m_searchTasks.isEmpty() ) {
        emit q->searchFinished( m_lastSearchTerm );
        emit q->placemarkSearchFinished();
    }
}

void MarbleRunnerManagerPrivate::cleanupReverseGeocodingTask( RunnerTask* task )
{
    m_reverseTasks.removeAll( task );
    mDebug() << "removing task " << m_reverseTasks.size() << " " << (long)task;
    if ( m_reverseTasks.isEmpty() ) {
        emit q->reverseGeocodingFinished();
    }
}

void MarbleRunnerManagerPrivate::cleanupRoutingTask( RunnerTask* task )
{
    m_routingTasks.removeAll( task );
    mDebug() << "removing task " << m_routingTasks.size() << " " << (long)task;
    if ( m_routingTasks.isEmpty() ) {
        if ( m_routingResult.isEmpty() ) {
            emit q->routeRetrieved( 0 );
        }

        emit q->routingFinished();
    }
}

void MarbleRunnerManagerPrivate::cleanupParsingTask( RunnerTask* task )
{
    m_parsingTasks.removeAll( task );
    mDebug() << "removing task " << m_parsingTasks.size() << " " << (long)task;

    if ( m_parsingTasks.isEmpty() ) {
        emit q->parsingFinished();
    }
}

MarbleRunnerManager::MarbleRunnerManager( const PluginManager* pluginManager, QObject *parent )
    : QObject( parent ), d( new MarbleRunnerManagerPrivate( this, pluginManager ) )
{
    if ( QThreadPool::globalInstance()->maxThreadCount() < 4 ) {
        QThreadPool::globalInstance()->setMaxThreadCount( 4 );
    }
}

MarbleRunnerManager::~MarbleRunnerManager()
{
    delete d;
}

void MarbleRunnerManager::findPlacemarks( const QString &searchTerm )
{
    if ( searchTerm == d->m_lastSearchTerm ) {
      emit searchResultChanged( d->m_model );
      emit searchResultChanged( d->m_placemarkContainer );
      emit searchFinished( searchTerm );
      emit placemarkSearchFinished();
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

    if ( searchTerm.trimmed().isEmpty() ) {
        emit searchFinished( searchTerm );
        emit placemarkSearchFinished();
        return;
    }

    QList<RunnerPlugin*> plugins = d->plugins( RunnerPlugin::Search );
    foreach( RunnerPlugin* plugin, plugins ) {
        SearchTask* task = new SearchTask( plugin, this, d->m_marbleModel, searchTerm );
        connect( task, SIGNAL( finished( RunnerTask* ) ), this, SLOT( cleanupSearchTask( RunnerTask* ) ) );
        d->m_searchTasks << task;
        mDebug() << "search task " << plugin->nameId() << " " << (long)task;
        QThreadPool::globalInstance()->start( task );
    }

    if ( plugins.isEmpty() ) {
        d->cleanupSearchTask( 0 );
    }
}

void MarbleRunnerManagerPrivate::addSearchResult( QVector<GeoDataPlacemark*> result )
{
    mDebug() << "Runner reports" << result.size() << " search results";
    if( result.isEmpty() )
        return;

    m_modelMutex.lock();
    int start = m_placemarkContainer.size();
    m_placemarkContainer << result;
    m_model->addPlacemarks( start, result.size() );
    m_modelMutex.unlock();
    emit q->searchResultChanged( m_model );
    emit q->searchResultChanged( m_placemarkContainer );
}

QVector<GeoDataPlacemark*> MarbleRunnerManager::searchPlacemarks( const QString &searchTerm ) {
    QEventLoop localEventLoop;
    QTimer watchdog;
    watchdog.setSingleShot(true);
    connect( &watchdog, SIGNAL(timeout()),
             &localEventLoop, SLOT(quit()));
    connect(this, SIGNAL(placemarkSearchFinished()),
            &localEventLoop, SLOT(quit()), Qt::QueuedConnection );

    watchdog.start( d->m_watchdogTimer );
    findPlacemarks( searchTerm );
    localEventLoop.exec();
    return d->m_placemarkContainer;
}

void MarbleRunnerManager::setModel( MarbleModel * model )
{
    // TODO: Terminate runners which are making use of the map.
    d->m_marbleModel = model;
}

void MarbleRunnerManager::reverseGeocoding( const GeoDataCoordinates &coordinates )
{
    d->m_reverseTasks.clear();
    d->m_reverseGeocodingResult.clear();
    d->m_reverseGeocodingResults.removeAll( coordinates );
    QList<RunnerPlugin*> plugins = d->plugins( RunnerPlugin::ReverseGeocoding );
    foreach( RunnerPlugin* plugin, plugins ) {
        ReverseGeocodingTask* task = new ReverseGeocodingTask( plugin, this, d->m_marbleModel, coordinates );
        connect( task, SIGNAL( finished( RunnerTask* ) ), this, SLOT( cleanupReverseGeocodingTask(RunnerTask*) ) );
        mDebug() << "reverse task " << plugin->nameId() << " " << (long)task;
        d->m_reverseTasks << task;
        QThreadPool::globalInstance()->start( task );
    }

    if ( plugins.isEmpty() ) {
        emit reverseGeocodingFinished( coordinates, GeoDataPlacemark() );
        d->cleanupReverseGeocodingTask( 0 );
    }
}

void MarbleRunnerManagerPrivate::addReverseGeocodingResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark )
{
    if ( !m_reverseGeocodingResults.contains( coordinates ) && !placemark.address().isEmpty() ) {
        m_reverseGeocodingResults.push_back( coordinates );
        m_reverseGeocodingResult = placemark.address();
        emit q->reverseGeocodingFinished( coordinates, placemark );
    }

    if ( m_reverseTasks.isEmpty() ) {
        emit q->reverseGeocodingFinished();
    }
}

QString MarbleRunnerManager::searchReverseGeocoding( const GeoDataCoordinates &coordinates ) {
    QEventLoop localEventLoop;
    QTimer watchdog;
    watchdog.setSingleShot(true);
    connect( &watchdog, SIGNAL(timeout()),
             &localEventLoop, SLOT(quit()));
    connect(this, SIGNAL(reverseGeocodingFinished()),
            &localEventLoop, SLOT(quit()), Qt::QueuedConnection );

    watchdog.start( d->m_watchdogTimer );
    reverseGeocoding( coordinates );
    localEventLoop.exec();
    return d->m_reverseGeocodingResult;
}

void MarbleRunnerManager::retrieveRoute( const RouteRequest *request )
{
    RoutingProfile profile = request->routingProfile();

    d->m_routingTasks.clear();
    d->m_routingResult.clear();

    QList<RunnerPlugin*> plugins = d->plugins( RunnerPlugin::Routing );
    foreach( RunnerPlugin* plugin, plugins ) {
        if ( !profile.name().isEmpty() && !profile.pluginSettings().contains( plugin->nameId() ) ) {
            continue;
        }

        RoutingTask* task = new RoutingTask( plugin, this, d->m_marbleModel, request );
        connect( task, SIGNAL( finished( RunnerTask* ) ), this, SLOT( cleanupRoutingTask( RunnerTask* ) ) );
        mDebug() << "route task " << plugin->nameId() << " " << (long)task;
        d->m_routingTasks << task;
        QThreadPool::globalInstance()->start( task );
    }

    if ( plugins.isEmpty() ) {
        mDebug() << "No routing plugins found, cannot retrieve a route";
        d->cleanupRoutingTask( 0 );
    }
}

void MarbleRunnerManagerPrivate::addRoutingResult( GeoDataDocument* route )
{
    if ( route ) {
        mDebug() << "route retrieved";
        m_routingResult.push_back( route );
        emit q->routeRetrieved( route );
    }
}

QVector<GeoDataDocument*> MarbleRunnerManager::searchRoute( const RouteRequest *request ) {
    QEventLoop localEventLoop;
    QTimer watchdog;
    watchdog.setSingleShot(true);
    connect( &watchdog, SIGNAL(timeout()),
             &localEventLoop, SLOT(quit()));
    connect(this, SIGNAL(routingFinished()),
            &localEventLoop, SLOT(quit()), Qt::QueuedConnection );

    watchdog.start( d->m_watchdogTimer );
    retrieveRoute( request );
    localEventLoop.exec();
    return d->m_routingResult;
}

void MarbleRunnerManager::parseFile( const QString &fileName, DocumentRole role )
{
    QList<RunnerPlugin*> plugins = d->plugins( RunnerPlugin::Parsing );
    foreach( RunnerPlugin *plugin, plugins ) {
        ParsingTask *task = new ParsingTask( plugin, this, fileName, role );
        connect( task, SIGNAL( finished( RunnerTask* ) ), this, SLOT( cleanupParsingTask(RunnerTask*) ) );
        mDebug() << "parse task " << plugin->nameId() << " " << (long)task;
        d->m_parsingTasks << task;
        QThreadPool::globalInstance()->start( task );
    }

    if ( plugins.isEmpty() ) {
        emit parsingFinished( 0, "No plugin found");
        d->cleanupParsingTask( 0 );
    }
}

void MarbleRunnerManagerPrivate::addParsingResult( GeoDataDocument *document, const QString& error )
{
    if ( document || !error.isEmpty() ) {
        m_fileResult = document;
        emit q->parsingFinished( document, error );
    }
}

GeoDataDocument* MarbleRunnerManager::openFile( const QString &fileName, DocumentRole role ) {
    QEventLoop localEventLoop;
    QTimer watchdog;
    watchdog.setSingleShot(true);
    connect( &watchdog, SIGNAL(timeout()),
             &localEventLoop, SLOT(quit()));
    connect(this, SIGNAL(parsingFinished()),
            &localEventLoop, SLOT(quit()), Qt::QueuedConnection );

    watchdog.start( d->m_watchdogTimer );
    parseFile( fileName, role);
    localEventLoop.exec();
    return d->m_fileResult;
}

}

#include "MarbleRunnerManager.moc"
