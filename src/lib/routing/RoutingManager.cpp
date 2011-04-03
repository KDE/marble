//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutingManager.h"

#include "AlternativeRoutesModel.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "RouteRequest.h"
#include "RoutingModel.h"
#include "RoutingProfilesModel.h"
#include "MarbleRunnerManager.h"
#include "AdjustNavigation.h"
#include "GeoWriter.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"
#include "GeoDataFolder.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "PositionTracking.h"
#include "PluginManager.h"
#include "PositionProviderPlugin.h"

#include <QtCore/QFile>
#include <QtGui/QMessageBox>
#include <QMutexLocker>

namespace Marble
{

class RoutingManagerPrivate
{
public:
    RoutingManager* q;

    RouteRequest *m_routeRequest;

    RoutingModel *m_routingModel;

    RoutingProfilesModel *m_profilesModel;

    MarbleModel *m_marbleModel;

    AlternativeRoutesModel* m_alternativeRoutesModel;

    bool m_workOffline;

    MarbleRunnerManager* m_runnerManager;

    bool m_haveRoute;

    AdjustNavigation *m_adjustNavigation;

    bool m_guidanceModeEnabled;

    QMutex m_fileMutex;

    bool m_shutdownPositionTracking;

    RoutingManagerPrivate( MarbleModel *marbleModel, RoutingManager* manager, QObject *parent );

    GeoDataFolder* routeRequest() const;

    QString stateFile( const QString &name = QString( "route.kml" ) ) const;

    void saveRoute( const QString &filename );

    void loadRoute( const QString &filename );
};

RoutingManagerPrivate::RoutingManagerPrivate( MarbleModel *model, RoutingManager* manager, QObject *parent ) :
        q( manager ),
        m_routeRequest( new RouteRequest( manager ) ),
        m_routingModel( new RoutingModel( m_routeRequest, model ) ),
        m_profilesModel( new RoutingProfilesModel( model->pluginManager() ) ),
        m_marbleModel( model ),
        m_alternativeRoutesModel(new AlternativeRoutesModel( model, parent ) ),
        m_workOffline( false ),
        m_runnerManager( new MarbleRunnerManager( model->pluginManager(), q ) ),
        m_haveRoute( false ), m_adjustNavigation( 0 ),
        m_guidanceModeEnabled( false ),
        m_shutdownPositionTracking( false )
{
    // nothing to do
}

GeoDataFolder* RoutingManagerPrivate::routeRequest() const
{
    GeoDataFolder* result = new GeoDataFolder;
    result->setName( "Route Request" );
    for ( int i=0; i<m_routeRequest->size(); ++i ) {
        GeoDataPlacemark* placemark = new GeoDataPlacemark;
        placemark->setName( m_routeRequest->name( i ) );
        placemark->setCoordinate( GeoDataPoint( m_routeRequest->at( i ) ) );
        result->append( placemark );
    }

    return result;
}

QString RoutingManagerPrivate::stateFile( const QString &name) const
{
    QString const subdir = "routing";
    QDir dir( MarbleDirs::localPath() );
    if ( !dir.exists( subdir ) ) {
        if ( !dir.mkdir( subdir ) ) {
            mDebug() << "Unable to create dir " << dir.absoluteFilePath( subdir );
            return dir.absolutePath();
        }
    }

    if ( !dir.cd( subdir ) ) {
        mDebug() << "Cannot change into " << dir.absoluteFilePath( subdir );
    }

    return dir.absoluteFilePath( name );
}

void RoutingManagerPrivate::saveRoute(const QString &filename)
{
    GeoWriter writer;
    writer.setDocumentType( "http://earth.google.com/kml/2.2" );

    QMutexLocker locker( &m_fileMutex );
    QFile file( filename );
    if ( !file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    {
        mDebug() << "Cannot write to " << file.fileName();
        return;
    }

    GeoDataDocument container;
    GeoDataFolder* request = routeRequest();
    if ( request ) {
        container.append( request );
    }

    GeoDataDocument *route = m_alternativeRoutesModel->currentRoute();
    if ( route ) {
        container.append( new GeoDataDocument( *route ) );
    }

    if ( !writer.write( &file, &container ) ) {
        mDebug() << "Can not write route state to " << file.fileName();
    }
    file.close();
}

void RoutingManagerPrivate::loadRoute(const QString &filename)
{
    QFile file( filename );
    if ( !file.open( QIODevice::ReadOnly ) ) {
        mDebug() << "Can not read route state from " << file.fileName();
        return;
    }

    GeoDataParser parser( GeoData_KML );
    if ( !parser.read( &file ) ) {
        mDebug() << "Could not parse file: " << parser.errorString();
        return;
    }

    GeoDocument *doc = parser.releaseDocument();
    GeoDataDocument* container = dynamic_cast<GeoDataDocument*>( doc );
    if ( container && container->size() == 2 ) {
        GeoDataFolder* viaPoints = dynamic_cast<GeoDataFolder*>( &container->first() );
        if ( viaPoints ) {
            QVector<GeoDataPlacemark*> placemarks = viaPoints->placemarkList();
            for( int i=0; i<placemarks.size(); ++i ) {
                if ( i < m_routeRequest->size() ) {
                    m_routeRequest->setPosition( i, placemarks[i]->coordinate() );
                } else {
                    m_routeRequest->append( placemarks[i]->coordinate() );
                }
                m_routeRequest->setName( m_routeRequest->size()-1, placemarks[i]->name() );
            }

            for ( int i=placemarks.size(); i<m_routeRequest->size(); ++i ) {
                m_routeRequest->remove( i );
            }
        } else {
            mDebug() << "Expected a GeoDataDocument, didn't get one though";
        }

        GeoDataDocument* route = dynamic_cast<GeoDataDocument*>(&container->last());
        if ( route ) {
            m_alternativeRoutesModel->addRoute( route, AlternativeRoutesModel::Instant );
            m_alternativeRoutesModel->setCurrentRoute( 0 );
        } else {
            mDebug() << "Expected a GeoDataDocument, didn't get one though";
        }
    } else {
        mDebug() << "Expected a GeoDataDocument, didn't get one though";
    }

    file.close();
}

RoutingManager::RoutingManager( MarbleModel *marbleModel, QObject *parent ) : QObject( parent ),
        d( new RoutingManagerPrivate( marbleModel, this, this ) )
{
    connect( d->m_runnerManager, SIGNAL( routeRetrieved( GeoDataDocument* ) ),
             this, SLOT( retrieveRoute( GeoDataDocument* ) ) );
    connect( d->m_alternativeRoutesModel, SIGNAL( currentRouteChanged( GeoDataDocument* ) ),
             d->m_routingModel, SLOT( setCurrentRoute( GeoDataDocument* ) ) );
    connect( d->m_routingModel, SIGNAL( deviatedFromRoute( bool ) ),
             this, SLOT( recalculateRoute( bool ) ) );
}

RoutingManager::~RoutingManager()
{
    delete d;
}

RoutingProfilesModel *RoutingManager::profilesModel()
{
    return d->m_profilesModel;
}

RoutingModel *RoutingManager::routingModel()
{
    return d->m_routingModel;
}

void RoutingManager::retrieveRoute( RouteRequest *route )
{
    d->m_routeRequest = route;
    updateRoute();
}

RouteRequest* RoutingManager::routeRequest()
{
    return d->m_routeRequest;
}

void RoutingManager::updateRoute()
{
    if ( !d->m_routeRequest ) {
        return;
    }

    d->m_haveRoute = false;

    int realSize = 0;
    for ( int i = 0; i < d->m_routeRequest->size(); ++i ) {
        // Sort out dummy targets
        if ( d->m_routeRequest->at( i ).longitude() != 0.0 && d->m_routeRequest->at( i ).latitude() != 0.0 ) {
            ++realSize;
        }
    }

    d->m_alternativeRoutesModel->newRequest( d->m_routeRequest );
    if ( realSize > 1 ) {
        emit stateChanged( RoutingManager::Downloading, d->m_routeRequest );
        d->m_runnerManager->setWorkOffline( d->m_workOffline );
        d->m_runnerManager->retrieveRoute( d->m_routeRequest );
    } else {
        d->m_routingModel->clear();
        emit stateChanged( RoutingManager::Retrieved, d->m_routeRequest );
    }
}

void RoutingManager::setWorkOffline( bool offline )
{
    d->m_workOffline = offline;
}

void RoutingManager::retrieveRoute( GeoDataDocument* route )
{
    if ( route ) {
        d->m_alternativeRoutesModel->addRoute( route );
    }

    if ( !d->m_haveRoute ) {
        d->m_haveRoute = route != 0;
        emit stateChanged( Retrieved, d->m_routeRequest );
    }

    emit routeRetrieved( route );
}

AlternativeRoutesModel* RoutingManager::alternativeRoutesModel()
{
    return d->m_alternativeRoutesModel;
}

void RoutingManager::setAdjustNavigation( AdjustNavigation* adjustNavigation )
{
    d->m_adjustNavigation = adjustNavigation;
}

AdjustNavigation* RoutingManager::adjustNavigation()
{
    return d->m_adjustNavigation;
}

void RoutingManager::writeSettings() const
{
    d->saveRoute( d->stateFile() );
}

void RoutingManager::saveRoute( const QString &filename ) const
{
    d->saveRoute( filename );
}

void RoutingManager::loadRoute( const QString &filename )
{
    d->loadRoute( filename );
}

void RoutingManager::readSettings()
{
    d->loadRoute( d->stateFile() );
    if ( d->m_routeRequest && d->m_profilesModel->rowCount() ) {
        d->m_routeRequest->setRoutingProfile( d->m_profilesModel->profiles().at( 0 ) );
    }
}

void RoutingManager::setGuidanceModeEnabled( bool enabled )
{
    d->m_guidanceModeEnabled = enabled;

    if ( enabled ) {
        d->saveRoute( d->stateFile( "guidance.kml" ) );
    } else {
        d->loadRoute( d->stateFile( "guidance.kml" ) );
    }

    PositionTracking* tracking = d->m_marbleModel->positionTracking();
    PositionProviderPlugin* plugin = tracking->positionProviderPlugin();
    if ( !plugin && enabled ) {
        PluginManager* pluginManager = d->m_marbleModel->pluginManager();
        QList<PositionProviderPlugin*> plugins = pluginManager->createPositionProviderPlugins();
        if ( plugins.size() > 0 ) {
            plugin = plugins.takeFirst();
        }
        qDeleteAll( plugins );
        tracking->setPositionProviderPlugin( plugin );
        d->m_shutdownPositionTracking = true;
    } else if ( plugin && !enabled && d->m_shutdownPositionTracking ) {
        d->m_shutdownPositionTracking = false;
        tracking->setPositionProviderPlugin( 0 );
    }

    adjustNavigation()->setAutoZoom( enabled );
    adjustNavigation()->setRecenter( enabled ? AdjustNavigation::AlwaysRecenter : 0 );
}

void RoutingManager::recalculateRoute( bool deviated )
{
    if ( d->m_guidanceModeEnabled && deviated && d->m_routeRequest ) {
        for ( int i=d->m_routeRequest->size()-3; i>=0; --i ) {
            if ( d->m_routeRequest->visited( i ) ) {
                d->m_routeRequest->remove( i );
            }
        }

        if ( d->m_routeRequest->size() == 2 && d->m_routeRequest->visited( 0 ) && !d->m_routeRequest->visited( 1 ) ) {
            d->m_routeRequest->setPosition( 0, d->m_marbleModel->positionTracking()->currentLocation() );
            updateRoute();
        } else if ( d->m_routeRequest->size() != 0 && !d->m_routeRequest->visited( d->m_routeRequest->size()-1 ) ) {
            d->m_routeRequest->insert( 0, d->m_marbleModel->positionTracking()->currentLocation() );
            updateRoute();
        }
    }
}

void RoutingManager::reverseRoute()
{
    d->m_routeRequest->reverse();
    updateRoute();
}

} // namespace Marble

#include "RoutingManager.moc"
