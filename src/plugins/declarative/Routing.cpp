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

class RoutingPrivate
{
public:
    RoutingPrivate();

    Marble::MarbleWidget* m_marbleWidget;

    QAbstractItemModel* m_routeRequestModel;

    QMap<QString, RoutingProfile> m_profiles;

    QString m_routingProfile;
};

RoutingPrivate::RoutingPrivate() :
    m_marbleWidget( 0 ), m_routeRequestModel( 0 )
{
    // nothing to do
}

Routing::Routing( QObject* parent) :
    QObject( parent ), d( new RoutingPrivate )
{
    // nothing to do
}

Routing::~Routing()
{
    delete d;
}

QObject* Routing::routeRequestModel()
{
    if ( !d->m_routeRequestModel && d->m_marbleWidget ) {
        RouteRequest* request = d->m_marbleWidget->model()->routingManager()->routeRequest();
        d->m_routeRequestModel = new RouteRequestModel( request, this );
    }

    return d->m_routeRequestModel;
}

QObject* Routing::waypointModel()
{
    return d->m_marbleWidget ? d->m_marbleWidget->model()->routingManager()->routingModel() : 0;
}

void Routing::setMarbleWidget( Marble::MarbleWidget* widget )
{
    d->m_marbleWidget = widget;

    if ( d->m_marbleWidget ) {
        QString const routeFile = MarbleDirs::path( "routing/route.kml" );
        if ( !routeFile.isEmpty() ) {
            d->m_marbleWidget->model()->routingManager()->loadRoute( routeFile );
        }

        d->m_marbleWidget->model()->routingManager()->profilesModel()->loadDefaultProfiles();
        QList<Marble::RoutingProfile> profiles = d->m_marbleWidget->model()->routingManager()->profilesModel()->profiles();
        if ( profiles.size() == 4 ) {
            /** @todo FIXME: Restrictive assumptions on available plugins and certain profile loading implementation */
            d->m_profiles["Motorcar"] = profiles.at( 0 );
            d->m_profiles["Bicycle"] = profiles.at( 2 );
            d->m_profiles["Pedestrian"] = profiles.at( 3 );
        } else {
            qDebug() << "Unexpected size of default routing profiles: " << profiles.size();
        }
    }
}

QString Routing::routingProfile() const
{
    return d->m_routingProfile;
}

void Routing::setRoutingProfile( const QString & profile )
{
    if ( d->m_routingProfile != profile ) {
        d->m_routingProfile = profile;
        if ( d->m_marbleWidget ) {
            d->m_marbleWidget->model()->routingManager()->routeRequest()->setRoutingProfile( d->m_profiles[profile] );
        }
        emit routingProfileChanged();
    }
}

void Routing::addVia( qreal lon, qreal lat )
{
    if ( d->m_marbleWidget ) {
        RouteRequest* request = d->m_marbleWidget->model()->routingManager()->routeRequest();
        request->append( GeoDataCoordinates( lon, lat, 0.0, GeoDataCoordinates::Degree ) );
        updateRoute();
    }
}

void Routing::setVia( int index, qreal lon, qreal lat )
{
    if ( index < 0 || index > 200 || !d->m_marbleWidget ) {
        return;
    }

    RouteRequest* request = d->m_marbleWidget->model()->routingManager()->routeRequest();
    Q_ASSERT( request );
    if ( index < request->size() ) {
        request->append( GeoDataCoordinates( lon, lat, 0.0, GeoDataCoordinates::Degree ) );
    } else {
        for ( int i=request->size(); i<index; ++i ) {
            request->append( GeoDataCoordinates( 0.0, 0.0 ) );
        }
        request->append( GeoDataCoordinates( lon, lat, 0.0, GeoDataCoordinates::Degree ) );
    }

    updateRoute();
}

void Routing::removeVia( int index )
{
    if ( index < 0 || !d->m_marbleWidget ) {
        return;
    }

    RouteRequest* request = d->m_marbleWidget->model()->routingManager()->routeRequest();
    if ( index < request->size() ) {
        d->m_marbleWidget->model()->routingManager()->routeRequest()->remove( index );
    }
}

void Routing::clearRoute()
{
    if ( d->m_marbleWidget ) {
        d->m_marbleWidget->model()->routingManager()->clearRoute();
    }
}

void Routing::updateRoute()
{
    if ( d->m_marbleWidget ) {
        d->m_marbleWidget->model()->routingManager()->updateRoute();
    }
}

}
}

#include "Routing.moc"
