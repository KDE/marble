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

}
}

#include "Routing.moc"
