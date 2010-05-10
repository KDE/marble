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

#include "MarbleDebug.h"
#include "MarbleWidget.h"
#include "OrsRoutingProvider.h"
#include "RouteSkeleton.h"
#include "RoutingModel.h"

#include <QtGui/QMessageBox>

namespace Marble
{

class RoutingManagerPrivate
{
public:
    RoutingModel *m_routingModel;

    AbstractRoutingProvider *m_routingProvider;

    MarbleWidget *m_marbleWidget;

    RoutingManagerPrivate( MarbleWidget *widget, QObject *parent );

    RouteSkeleton *m_route;

    bool m_workOffline;
};

RoutingManagerPrivate::RoutingManagerPrivate( MarbleWidget *widget, QObject *parent ) :
        m_routingModel( new RoutingModel( parent ) ),
        m_routingProvider( new OrsRoutingProvider( parent ) ),
        m_marbleWidget( widget ), m_route( 0 ), m_workOffline( false )
{
    // nothing to do
}

RoutingManager::RoutingManager( MarbleWidget *widget, QObject *parent ) : QObject( parent ),
        d( new RoutingManagerPrivate( widget, this ) )
{
    connect( d->m_routingProvider, SIGNAL( routeRetrieved( AbstractRoutingProvider::Format, QByteArray ) ),
             this, SLOT( setRouteData( AbstractRoutingProvider::Format, QByteArray ) ) );
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

void RoutingManager::setRouteData( AbstractRoutingProvider::Format format, const QByteArray &data )
{
    /** @todo: switch to using GeoDataDocument* */
    Q_UNUSED( format );

    if ( data.size() ) {
        d->m_routingModel->importOpenGis( data );
        d->m_marbleWidget->repaint();
    } else {
        mDebug() << "Got an empty result instead of route data";
        QString message = tr("Sorry, the route could not be retrieved. Please try again later.");
        QMessageBox::warning( d->m_marbleWidget, "Route Error", message );
    }

    emit stateChanged( Retrieved, d->m_route );
}

void RoutingManager::updateRoute()
{
    if ( !d->m_workOffline && d->m_route && d->m_route->size() > 1 ) {
        int realSize = 0;
        for ( int i = 0; i < d->m_route->size(); ++i ) {
            // Sort out dummy targets
            if ( d->m_route->at( i ).longitude() != 0.0 && d->m_route->at( i ).latitude() != 0.0 ) {
                ++realSize;
            }
        }

        if ( realSize > 1 ) {
            emit stateChanged( Downloading, d->m_route );
            d->m_routingProvider->retrieveDirections( d->m_route );
        } else {
            d->m_routingModel->clear();
            emit stateChanged( Retrieved, d->m_route );
        }
    }
}

void RoutingManager::setWorkOffline( bool offline )
{
    d->m_workOffline = offline;
}

} // namespace Marble

#include "RoutingManager.moc"
