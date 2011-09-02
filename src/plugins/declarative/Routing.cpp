//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "Routing.h"

#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "MarbleDirs.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"
#include "routing/RouteRequest.h"
#include "RouteRequestModel.h"

namespace Marble
{
namespace Declarative
{

Routing::Routing( QObject* parent) : QObject( parent ),
    m_marbleWidget( 0 ), m_routeRequestModel( 0 )
{
    // nothing to do
}

QObject* Routing::routeRequestModel()
{
    if ( !m_routeRequestModel && m_marbleWidget ) {
        RouteRequest* request = m_marbleWidget->model()->routingManager()->routeRequest();
        m_routeRequestModel = new RouteRequestModel( request, this );
    }

    return m_routeRequestModel;
}

QObject* Routing::waypointModel()
{
    return m_marbleWidget ? m_marbleWidget->model()->routingManager()->routingModel() : 0;
}

void Routing::setMarbleWidget( Marble::MarbleWidget* widget )
{
    m_marbleWidget = widget;

    if ( m_marbleWidget ) {
        QString const routeFile = MarbleDirs::path( "routing/route.kml" );
        if ( !routeFile.isEmpty() ) {
            m_marbleWidget->model()->routingManager()->loadRoute( routeFile );
        }
    }
}

void Routing::addVia( qreal lon, qreal lat )
{
    if ( m_marbleWidget ) {
        RouteRequest* request = m_marbleWidget->model()->routingManager()->routeRequest();
        request->append( GeoDataCoordinates( lon, lat, 0.0, GeoDataCoordinates::Degree ) );
        m_marbleWidget->model()->routingManager()->updateRoute();
    }
}

void Routing::setVia( int index, qreal lon, qreal lat )
{
    if ( index < 0 || index > 200 || !m_marbleWidget ) {
        return;
    }

    RouteRequest* request = m_marbleWidget->model()->routingManager()->routeRequest();
    Q_ASSERT( request );
    if ( index < request->size() ) {
        request->append( GeoDataCoordinates( lon, lat, 0.0, GeoDataCoordinates::Degree ) );
    } else {
        for ( int i=request->size(); i<index; ++i ) {
            request->append( GeoDataCoordinates( 0.0, 0.0 ) );
        }
        request->append( GeoDataCoordinates( lon, lat, 0.0, GeoDataCoordinates::Degree ) );
    }
    m_marbleWidget->model()->routingManager()->updateRoute();
}

void Routing::removeVia( int index )
{
    if ( index < 0 || !m_marbleWidget ) {
        return;
    }

    RouteRequest* request = m_marbleWidget->model()->routingManager()->routeRequest();
    if ( index < request->size() ) {
        m_marbleWidget->model()->routingManager()->routeRequest()->remove( index );
    }
}

void Routing::clearRoute()
{
    if ( m_marbleWidget ) {
        m_marbleWidget->model()->routingManager()->clearRoute();
    }
}


}
}

#include "Routing.moc"
