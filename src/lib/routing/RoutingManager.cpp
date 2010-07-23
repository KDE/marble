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

#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "RouteSkeleton.h"
#include "RoutingModel.h"
#include "MarbleRunnerManager.h"

#include <QtGui/QMessageBox>

namespace Marble
{

class RoutingManagerPrivate
{
public:
    RoutingManager* q;

    RoutingModel *m_routingModel;

    MarbleWidget *m_marbleWidget;

    RouteSkeleton *m_route;

    bool m_workOffline;

    MarbleRunnerManager* m_runnerManager;

    bool m_haveRoute;

    RoutingManagerPrivate( MarbleWidget *widget, RoutingManager* manager, QObject *parent );
};

RoutingManagerPrivate::RoutingManagerPrivate( MarbleWidget *widget, RoutingManager* manager, QObject *parent ) :
        q( manager ), m_routingModel( new RoutingModel( parent ) ),
        m_marbleWidget( widget ), m_route( 0 ), m_workOffline( false ),
        m_runnerManager( new MarbleRunnerManager( widget->model()->pluginManager(), q ) ),
        m_haveRoute( false )
{
    // nothing to do
}

RoutingManager::RoutingManager( MarbleWidget *widget, QObject *parent ) : QObject( parent ),
        d( new RoutingManagerPrivate( widget, this, this ) )
{
    connect( d->m_runnerManager, SIGNAL( routeRetrieved( GeoDataDocument* ) ),
             this, SLOT( retrieveRoute( GeoDataDocument* ) ) );
}

RoutingManager::~RoutingManager()
{
    delete d;
}

RoutingModel *RoutingManager::routingModel()
{
    return d->m_routingModel;
}

void RoutingManager::retrieveRoute( RouteSkeleton *route )
{
    d->m_route = route;
    updateRoute();
}

void RoutingManager::updateRoute()
{
    Q_ASSERT( d->m_route );
    d->m_haveRoute = false;

    int realSize = 0;
    for ( int i = 0; i < d->m_route->size(); ++i ) {
        // Sort out dummy targets
        if ( d->m_route->at( i ).longitude() != 0.0 && d->m_route->at( i ).latitude() != 0.0 ) {
            ++realSize;
        }
    }

    if ( realSize > 1 ) {
        emit stateChanged( RoutingManager::Downloading, d->m_route );
        d->m_runnerManager->setWorkOffline( d->m_workOffline );
        d->m_runnerManager->retrieveRoute( d->m_route );
    } else {
        d->m_routingModel->clear();
        emit stateChanged( RoutingManager::Retrieved, d->m_route );
    }
}

void RoutingManager::setWorkOffline( bool offline )
{
    d->m_workOffline = offline;
}

void RoutingManager::retrieveRoute( GeoDataDocument* route )
{
    if ( !d->m_haveRoute ) {
        d->m_haveRoute = true;
        emit stateChanged( Retrieved, d->m_route );
    }

    emit routeRetrieved( route );
}

} // namespace Marble

#include "RoutingManager.moc"
