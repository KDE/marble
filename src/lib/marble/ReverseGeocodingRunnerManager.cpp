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

#include "ReverseGeocodingRunnerManager.h"

#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "GeoDataCoordinates.h"
#include "GeoDataPlacemark.h"
#include "Planet.h"
#include "PluginManager.h"
#include "ReverseGeocodingRunnerPlugin.h"
#include "RunnerTask.h"

#include <QList>
#include <QThreadPool>
#include <QTimer>

namespace Marble
{

class MarbleModel;

class Q_DECL_HIDDEN ReverseGeocodingRunnerManager::Private
{
public:
    Private( ReverseGeocodingRunnerManager *parent, const MarbleModel *marbleModel );

    QList<const ReverseGeocodingRunnerPlugin *> plugins( const QList<const ReverseGeocodingRunnerPlugin *> &plugins ) const;

    void addReverseGeocodingResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark );
    void cleanupReverseGeocodingTask( ReverseGeocodingTask *task );

    ReverseGeocodingRunnerManager *const q;
    const MarbleModel *const m_marbleModel;
    const PluginManager* m_pluginManager;
    QList<ReverseGeocodingTask*> m_reverseTasks;
    QVector<GeoDataCoordinates> m_reverseGeocodingResults;
    QString m_reverseGeocodingResult;
};

ReverseGeocodingRunnerManager::Private::Private( ReverseGeocodingRunnerManager *parent, const MarbleModel *marbleModel ) :
    q( parent ),
    m_marbleModel( marbleModel ),
    m_pluginManager( marbleModel->pluginManager() )
{
    qRegisterMetaType<GeoDataPlacemark>( "GeoDataPlacemark" );
    qRegisterMetaType<GeoDataCoordinates>( "GeoDataCoordinates" );
}

QList<const ReverseGeocodingRunnerPlugin *> ReverseGeocodingRunnerManager::Private::plugins( const QList<const ReverseGeocodingRunnerPlugin *> &plugins ) const
{
    QList<const ReverseGeocodingRunnerPlugin *> result;

    foreach( const ReverseGeocodingRunnerPlugin *plugin, plugins ) {
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

void ReverseGeocodingRunnerManager::Private::addReverseGeocodingResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark )
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

void ReverseGeocodingRunnerManager::Private::cleanupReverseGeocodingTask( ReverseGeocodingTask *task )
{
    m_reverseTasks.removeAll( task );
    mDebug() << "removing task " << m_reverseTasks.size() << " " << (quintptr)task;
    if ( m_reverseTasks.isEmpty() ) {
        emit q->reverseGeocodingFinished();
    }
}

ReverseGeocodingRunnerManager::ReverseGeocodingRunnerManager( const MarbleModel *marbleModel, QObject *parent ) :
    QObject( parent ),
    d( new Private( this, marbleModel ) )
{
    if ( QThreadPool::globalInstance()->maxThreadCount() < 4 ) {
        QThreadPool::globalInstance()->setMaxThreadCount( 4 );
    }
}

ReverseGeocodingRunnerManager::~ReverseGeocodingRunnerManager()
{
    delete d;
}

void ReverseGeocodingRunnerManager::reverseGeocoding( const GeoDataCoordinates &coordinates )
{
    d->m_reverseTasks.clear();
    d->m_reverseGeocodingResult.clear();
#if QT_VERSION >= 0x050400
    d->m_reverseGeocodingResults.removeAll( coordinates );
#else
    QVector<GeoDataCoordinates> &vector = d->m_reverseGeocodingResults;
    QVector<GeoDataCoordinates>::iterator it = vector.begin();

    while (it != vector.end()) {
        if (*it == coordinates) {
            it = vector.erase(it);
        } else {
            ++it;
        }
    }
#endif
    QList<const ReverseGeocodingRunnerPlugin*> plugins = d->plugins( d->m_pluginManager->reverseGeocodingRunnerPlugins() );
    foreach( const ReverseGeocodingRunnerPlugin* plugin, plugins ) {
        ReverseGeocodingTask* task = new ReverseGeocodingTask( plugin->newRunner(), this, d->m_marbleModel, coordinates );
        connect( task, SIGNAL(finished(ReverseGeocodingTask*)), this, SLOT(cleanupReverseGeocodingTask(ReverseGeocodingTask*)) );
        mDebug() << "reverse task " << plugin->nameId() << " " << (quintptr)task;
        d->m_reverseTasks << task;
    }

    foreach( ReverseGeocodingTask* task, d->m_reverseTasks ) {
        QThreadPool::globalInstance()->start( task );
    }

    if ( plugins.isEmpty() ) {
        GeoDataPlacemark anonymous;
        anonymous.setCoordinate( coordinates );
        emit reverseGeocodingFinished( coordinates, anonymous );
        d->cleanupReverseGeocodingTask( 0 );
    }
}

QString ReverseGeocodingRunnerManager::searchReverseGeocoding( const GeoDataCoordinates &coordinates, int timeout ) {
    QEventLoop localEventLoop;
    QTimer watchdog;
    watchdog.setSingleShot(true);
    connect( &watchdog, SIGNAL(timeout()),
             &localEventLoop, SLOT(quit()));
    connect(this, SIGNAL(reverseGeocodingFinished()),
            &localEventLoop, SLOT(quit()), Qt::QueuedConnection );

    watchdog.start( timeout );
    reverseGeocoding( coordinates );
    localEventLoop.exec();
    return d->m_reverseGeocodingResult;
}

}

#include "moc_ReverseGeocodingRunnerManager.cpp"
