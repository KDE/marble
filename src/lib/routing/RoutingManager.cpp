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
#include "YoursRoutingProvider.h"
#include "RouteSkeleton.h"
#include "RoutingModel.h"

#include <QtGui/QMessageBox>

namespace Marble
{

class RoutingManagerPrivate
{
public:
    RoutingManager* q;

    RoutingModel *m_routingModel;

    AbstractRoutingProvider *m_orsProvider;

    AbstractRoutingProvider *m_yoursProvider;

    MarbleWidget *m_marbleWidget;

    RouteSkeleton *m_route;

    bool m_workOffline;

    RoutingManagerPrivate( MarbleWidget *widget, RoutingManager* manager, QObject *parent );

    void updateRoute( AbstractRoutingProvider* provider );
};

RoutingManagerPrivate::RoutingManagerPrivate( MarbleWidget *widget, RoutingManager* manager, QObject *parent ) :
        q( manager ), m_routingModel( new RoutingModel( parent ) ),
        m_orsProvider( new OrsRoutingProvider( parent ) ),
        m_yoursProvider( new YoursRoutingProvider( parent ) ),
        m_marbleWidget( widget ), m_route( 0 ), m_workOffline( false )
{
    // nothing to do
}

void RoutingManagerPrivate::updateRoute( AbstractRoutingProvider* provider )
{
    if ( !m_workOffline && m_route && m_route->size() > 1 ) {
        int realSize = 0;
        for ( int i = 0; i < m_route->size(); ++i ) {
            // Sort out dummy targets
            if ( m_route->at( i ).longitude() != 0.0 && m_route->at( i ).latitude() != 0.0 ) {
                ++realSize;
            }
        }

        if ( realSize > 1 ) {
            emit q->stateChanged( RoutingManager::Downloading, m_route );
            provider->retrieveDirections( m_route );
        } else {
            m_routingModel->clear();
            emit q->stateChanged( RoutingManager::Retrieved, m_route );
        }
    }
}

RoutingManager::RoutingManager( MarbleWidget *widget, QObject *parent ) : QObject( parent ),
        d( new RoutingManagerPrivate( widget, this, this ) )
{
    connect( d->m_orsProvider, SIGNAL( routeRetrieved( AbstractRoutingProvider::Format, QByteArray ) ),
             this, SLOT( setRouteData( AbstractRoutingProvider::Format, QByteArray ) ) );
    connect( d->m_yoursProvider, SIGNAL( routeRetrieved( AbstractRoutingProvider::Format, QByteArray ) ),
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

    if ( data.size() ) {
        if ( format == AbstractRoutingProvider::OpenGIS ) {
            if ( !d->m_routingModel->importOpenGis( data ) ) {
                mDebug() << " Invalid ORS route, trying YOURS instead";
                d->updateRoute( d->m_yoursProvider );
            }
        } else if ( format == AbstractRoutingProvider::KML ) {
            d->m_routingModel->importKml( data );
        } else {
            mDebug() << "Gpx format import for routing is not implemented yet";
        }
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
    d->updateRoute( d->m_orsProvider );
}

void RoutingManager::setWorkOffline( bool offline )
{
    d->m_workOffline = offline;
}

} // namespace Marble

#include "RoutingManager.moc"
