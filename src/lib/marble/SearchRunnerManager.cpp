//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>
// Copyright 2010 Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2010-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "SearchRunnerManager.h"

#include "MarblePlacemarkModel.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "MarbleMath.h"
#include "Planet.h"
#include "GeoDataPlacemark.h"
#include "PluginManager.h"
#include "ParseRunnerPlugin.h"
#include "ReverseGeocodingRunnerPlugin.h"
#include "RoutingRunnerPlugin.h"
#include "SearchRunnerPlugin.h"
#include "RunnerTask.h"
#include "routing/RouteRequest.h"
#include "routing/RoutingProfilesModel.h"

#include <QObject>
#include <QString>
#include <QVector>
#include <QThreadPool>
#include <QTimer>
#include <QMutex>

namespace Marble
{

class MarbleModel;

class Q_DECL_HIDDEN SearchRunnerManager::Private
{
public:
    Private( SearchRunnerManager *parent, const MarbleModel *marbleModel );

    template<typename T>
    QList<T*> plugins( const QList<T*> &plugins ) const;

    void addSearchResult( const QVector<GeoDataPlacemark *> &result );
    void cleanupSearchTask( SearchTask *task );

    SearchRunnerManager *const q;
    const MarbleModel *const m_marbleModel;
    const PluginManager* m_pluginManager;
    QString m_lastSearchTerm;
    GeoDataLatLonBox m_lastPreferredBox;
    QMutex m_modelMutex;
    MarblePlacemarkModel m_model;
    QList<SearchTask *> m_searchTasks;
    QVector<GeoDataPlacemark *> m_placemarkContainer;
};

SearchRunnerManager::Private::Private( SearchRunnerManager *parent, const MarbleModel *marbleModel ) :
    q( parent ),
    m_marbleModel( marbleModel ),
    m_pluginManager( marbleModel->pluginManager() ),
    m_model( new MarblePlacemarkModel( parent ) )
{
    m_model.setPlacemarkContainer( &m_placemarkContainer );
    qRegisterMetaType<QVector<GeoDataPlacemark *> >( "QVector<GeoDataPlacemark*>" );
}

template<typename T>
QList<T*> SearchRunnerManager::Private::plugins( const QList<T*> &plugins ) const
{
    QList<T*> result;
    foreach( T* plugin, plugins ) {
        if ( ( m_marbleModel && m_marbleModel->workOffline() && !plugin->canWorkOffline() ) ) {
            continue;
        }

        if ( !plugin->canWork() ) {
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

void SearchRunnerManager::Private::addSearchResult( const QVector<GeoDataPlacemark *> &result )
{
    mDebug() << "Runner reports" << result.size() << " search results";
    if( result.isEmpty() )
        return;

    m_modelMutex.lock();
    int start = m_placemarkContainer.size();
    int count = 0;
    bool distanceCompare = m_marbleModel->planet() != 0;
    for( int i=0; i<result.size(); ++i ) {
        bool same = false;
        for ( int j=0; j<m_placemarkContainer.size(); ++j ) {
            if ( distanceCompare &&
                 ( distanceSphere( result[i]->coordinate(),
                                   m_placemarkContainer[j]->coordinate() )
                   * m_marbleModel->planet()->radius() < 1 ) ) {
                same = true;
            }
        }
        if ( !same ) {
            m_placemarkContainer.append( result[i] );
            ++count;
        }
    }
    m_model.addPlacemarks( start, count );
    m_modelMutex.unlock();
    emit q->searchResultChanged( &m_model );
    emit q->searchResultChanged( m_placemarkContainer );
}

void SearchRunnerManager::Private::cleanupSearchTask( SearchTask *task )
{
    m_searchTasks.removeAll( task );
    mDebug() << "removing search task" << m_searchTasks.size() << (quintptr)task;
    if ( m_searchTasks.isEmpty() ) {
        if( m_placemarkContainer.isEmpty() ) {
            emit q->searchResultChanged( &m_model );
            emit q->searchResultChanged( m_placemarkContainer );
        }
        emit q->searchFinished( m_lastSearchTerm );
        emit q->placemarkSearchFinished();
    }
}

SearchRunnerManager::SearchRunnerManager( const MarbleModel *marbleModel, QObject *parent ) :
    QObject( parent ),
    d( new Private( this, marbleModel ) )
{
    if ( QThreadPool::globalInstance()->maxThreadCount() < 4 ) {
        QThreadPool::globalInstance()->setMaxThreadCount( 4 );
    }
}

SearchRunnerManager::~SearchRunnerManager()
{
    delete d;
}

void SearchRunnerManager::findPlacemarks( const QString &searchTerm, const GeoDataLatLonBox &preferred )
{
    if ( searchTerm == d->m_lastSearchTerm && preferred == d->m_lastPreferredBox ) {
      emit searchResultChanged( &d->m_model );
      emit searchResultChanged( d->m_placemarkContainer );
      emit searchFinished( searchTerm );
      emit placemarkSearchFinished();
      return;
    }

    d->m_lastSearchTerm = searchTerm;
    d->m_lastPreferredBox = preferred;

    d->m_searchTasks.clear();

    d->m_modelMutex.lock();
    d->m_model.removePlacemarks( "PlacemarkRunnerManager", 0, d->m_placemarkContainer.size() );
    qDeleteAll( d->m_placemarkContainer );
    d->m_placemarkContainer.clear();
    d->m_modelMutex.unlock();
    emit searchResultChanged( &d->m_model );

    if ( searchTerm.trimmed().isEmpty() ) {
        emit searchFinished( searchTerm );
        emit placemarkSearchFinished();
        return;
    }

    QList<const SearchRunnerPlugin *> plugins = d->plugins( d->m_pluginManager->searchRunnerPlugins() );
    foreach( const SearchRunnerPlugin *plugin, plugins ) {
        SearchTask *task = new SearchTask( plugin->newRunner(), this, d->m_marbleModel, searchTerm, preferred );
        connect( task, SIGNAL(finished(SearchTask*)), this, SLOT(cleanupSearchTask(SearchTask*)) );
        d->m_searchTasks << task;
        mDebug() << "search task " << plugin->nameId() << " " << (quintptr)task;
    }

    foreach( SearchTask *task, d->m_searchTasks ) {
        QThreadPool::globalInstance()->start( task );
    }

    if ( plugins.isEmpty() ) {
        d->cleanupSearchTask( 0 );
    }
}

QVector<GeoDataPlacemark *> SearchRunnerManager::searchPlacemarks( const QString &searchTerm, const GeoDataLatLonBox &preferred, int timeout )
{
    QEventLoop localEventLoop;
    QTimer watchdog;
    watchdog.setSingleShot(true);
    connect( &watchdog, SIGNAL(timeout()),
             &localEventLoop, SLOT(quit()));
    connect(this, SIGNAL(placemarkSearchFinished()),
            &localEventLoop, SLOT(quit()), Qt::QueuedConnection );

    watchdog.start( timeout );
    findPlacemarks( searchTerm, preferred );
    localEventLoop.exec();
    return d->m_placemarkContainer;
}

}

#include "moc_SearchRunnerManager.cpp"
