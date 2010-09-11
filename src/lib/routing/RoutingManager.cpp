//
// This file is part of the Marble Desktop Globe.
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
#include "MarbleRunnerManager.h"
#include "AdjustNavigation.h"
#include "GeoWriter.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"
#include "GeoDataFolder.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"

#include <QtCore/QFile>
#include <QtGui/QMessageBox>
#include <QMutexLocker>

namespace Marble
{

class RoutingManagerPrivate
{
public:
    RoutingManager* q;

    RoutingModel *m_routingModel;

    MarbleModel *m_marbleModel;

    AlternativeRoutesModel* m_alternativeRoutesModel;

    RouteRequest *m_routeRequest;

    bool m_workOffline;

    MarbleRunnerManager* m_runnerManager;

    bool m_haveRoute;

    AdjustNavigation *m_adjustNavigation;

    QMutex m_fileMutex;

    RoutingManagerPrivate( MarbleModel *marbleModel, RoutingManager* manager, QObject *parent );

    GeoDataFolder* routeRequest() const;

    QString stateFile() const;
};

RoutingManagerPrivate::RoutingManagerPrivate( MarbleModel *model, RoutingManager* manager, QObject *parent ) :
        q( manager ),
        m_routingModel( new RoutingModel( model ) ),
        m_marbleModel( model ),
        m_alternativeRoutesModel(new AlternativeRoutesModel( model, parent ) ),
        m_routeRequest( new RouteRequest( manager ) ),
        m_workOffline( false ),
        m_runnerManager( new MarbleRunnerManager( model->pluginManager(), q ) ),
        m_haveRoute( false ), m_adjustNavigation( 0 )
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

QString RoutingManagerPrivate::stateFile() const
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

    return dir.absoluteFilePath( "route.kml" );
}

RoutingManager::RoutingManager( MarbleModel *marbleModel, QObject *parent ) : QObject( parent ),
        d( new RoutingManagerPrivate( marbleModel, this, this ) )
{
    connect( d->m_runnerManager, SIGNAL( routeRetrieved( GeoDataDocument* ) ),
             this, SLOT( retrieveRoute( GeoDataDocument* ) ) );
    connect( d->m_alternativeRoutesModel, SIGNAL( currentRouteChanged( GeoDataDocument* ) ),
             d->m_routingModel, SLOT( setCurrentRoute( GeoDataDocument* ) ) );
}

RoutingManager::~RoutingManager()
{
    delete d;
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
    d->m_alternativeRoutesModel->addRoute( route );

    if ( !d->m_haveRoute ) {
        d->m_haveRoute = true;
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
    GeoWriter writer;
    writer.setDocumentType( "http://earth.google.com/kml/2.2" );

    QMutexLocker locker( &d->m_fileMutex );
    QFile file( d->stateFile() );
    if ( !file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    {
        mDebug() << "Cannot write to " << file.fileName();
        return;
    }

    GeoDataDocument container;
    GeoDataFolder* request = d->routeRequest();
    if ( request ) {
        container.append( request );
    }

    GeoDataDocument *route = d->m_alternativeRoutesModel->currentRoute();
    if ( route ) {
        container.append( new GeoDataDocument( *route ) );
    }

    if ( !writer.write( &file, container ) ) {
        mDebug() << "Can not write route state to " << file.fileName();
    }
    file.close();
}

void RoutingManager::readSettings()
{
    QFile file( d->stateFile() );
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
                if ( i < d->m_routeRequest->size() ) {
                    d->m_routeRequest->setPosition( i, placemarks[i]->coordinate() );
                } else {
                    d->m_routeRequest->append( placemarks[i]->coordinate() );
                }
                d->m_routeRequest->setName( d->m_routeRequest->size()-1, placemarks[i]->name() );
            }
        } else {
            mDebug() << "Expected a GeoDataDocument, didn't get one though";
        }

        GeoDataDocument* route = dynamic_cast<GeoDataDocument*>(&container->last());
        if ( route ) {
            alternativeRoutesModel()->addRoute( route, AlternativeRoutesModel::Instant );
            alternativeRoutesModel()->setCurrentRoute( 0 );
        } else {
            mDebug() << "Expected a GeoDataDocument, didn't get one though";
        }
    } else {
        mDebug() << "Expected a GeoDataDocument, didn't get one though";
    }

    file.close();
}

} // namespace Marble

#include "RoutingManager.moc"
