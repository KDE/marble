//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#include "Routing.h"

#include "MarbleDeclarativeWidget.h"
#include <MarbleMap.h>
#include <MarbleModel.h>
#include "MarbleDirs.h"
#include "routing/AlternativeRoutesModel.h"
#include "routing/RoutingManager.h"
#include "routing/RouteRequest.h"
#include "routing/RoutingProfilesModel.h"
#include <GeoPainter.h>
#include <routing/Route.h>

#include <QDebug>
#include <QPainter>

namespace Marble {

class RoutingPrivate
{
public:
    RoutingPrivate();

    MarbleMap* m_marbleMap;
    QMap<QString, Marble::RoutingProfile> m_profiles;
    QString m_routingProfile;
};

RoutingPrivate::RoutingPrivate() :
    m_marbleMap( nullptr )
{
    // nothing to do
}

Routing::Routing( QQuickItem *parent) :
    QQuickPaintedItem( parent ), d( new RoutingPrivate )
{
    // nothing to do
}

Routing::~Routing()
{
    delete d;
}

void Routing::paint(QPainter *painter)
{
    if (!d->m_marbleMap) {
        return;
    }

    QPaintDevice *paintDevice = painter->device();
    painter->end();
    {
        Marble::GeoPainter geoPainter(paintDevice, d->m_marbleMap->viewport(), d->m_marbleMap->mapQuality());

        RoutingManager const * const routingManager = d->m_marbleMap->model()->routingManager();
        GeoDataLineString const waypoints = routingManager->routingModel()->route().path();

        QPen standardRoutePen( routingManager->routeColorStandard() );
        standardRoutePen.setWidth( 5 );
        if ( routingManager->state() == RoutingManager::Downloading ) {
            standardRoutePen.setStyle( Qt::DotLine );
        }
        geoPainter.setPen( standardRoutePen );
        geoPainter.drawPolyline( waypoints );
    }

    painter->begin(paintDevice);
}

QObject* Routing::waypointModel()
{
    return d->m_marbleMap ? d->m_marbleMap->model()->routingManager()->routingModel() : 0;
}

void Routing::setMarbleMap( MarbleMap* marbleMap )
{
    d->m_marbleMap = marbleMap;

    if ( d->m_marbleMap ) {
        connect(d->m_marbleMap, SIGNAL(repaintNeeded(QRegion)), this, SLOT(update()));
        RoutingManager* routingManager = d->m_marbleMap->model()->routingManager();
        if (routingManager->profilesModel()->rowCount() == 0) {
            routingManager->profilesModel()->loadDefaultProfiles();
            routingManager->readSettings();
        }

        connect( routingManager, SIGNAL(stateChanged(RoutingManager::State)),
                 this, SIGNAL(hasRouteChanged()) );
        emit routingModelChanged();

        QList<Marble::RoutingProfile> profiles = routingManager->profilesModel()->profiles();
        if ( profiles.size() == 4 ) {
            /** @todo FIXME: Restrictive assumptions on available plugins and certain profile loading implementation */
            d->m_profiles["Motorcar"] = profiles.at( 0 );
            d->m_profiles["Bicycle"] = profiles.at( 2 );
            d->m_profiles["Pedestrian"] = profiles.at( 3 );
        } else {
            qDebug() << "Unexpected size of default routing profiles: " << profiles.size();
        }
    }

    emit marbleMapChanged();
    emit routingProfileChanged();
    emit hasRouteChanged();
}

MarbleMap *Routing::marbleMap()
{
    return d->m_marbleMap;
}

QString Routing::routingProfile() const
{
    return d->m_routingProfile;
}

void Routing::setRoutingProfile( const QString & profile )
{
    if ( d->m_routingProfile != profile ) {
        d->m_routingProfile = profile;
        if ( d->m_marbleMap ) {
            d->m_marbleMap->model()->routingManager()->routeRequest()->setRoutingProfile( d->m_profiles[profile] );
        }
        emit routingProfileChanged();
    }
}

bool Routing::hasRoute() const
{
    return d->m_marbleMap && d->m_marbleMap->model()->routingManager()->routingModel()->rowCount() > 0;
}

RoutingModel *Routing::routingModel()
{
    return d->m_marbleMap == 0 ? 0 : d->m_marbleMap->model()->routingManager()->routingModel();
}

void Routing::addVia( qreal lon, qreal lat )
{
    if ( d->m_marbleMap ) {
        Marble::RouteRequest* request = d->m_marbleMap->model()->routingManager()->routeRequest();
        request->append( Marble::GeoDataCoordinates( lon, lat, 0.0, Marble::GeoDataCoordinates::Degree ) );
        updateRoute();
    }
}

void Routing::addVia(Coordinate *coordinate)
{
    addVia(coordinate->longitude(), coordinate->latitude());
}

void Routing::setVia( int index, qreal lon, qreal lat )
{
    if ( index < 0 || index > 200 || !d->m_marbleMap ) {
        return;
    }

    Marble::RouteRequest* request = d->m_marbleMap->model()->routingManager()->routeRequest();
    Q_ASSERT( request );
    if ( index < request->size() ) {
        request->setPosition( index, Marble::GeoDataCoordinates( lon, lat, 0.0, Marble::GeoDataCoordinates::Degree ) );
    } else {
        for ( int i=request->size(); i<index; ++i ) {
            request->append( Marble::GeoDataCoordinates( 0.0, 0.0 ) );
        }
        request->append( Marble::GeoDataCoordinates( lon, lat, 0.0, Marble::GeoDataCoordinates::Degree ) );
    }

    updateRoute();
}

void Routing::removeVia( int index )
{
    if ( index < 0 || !d->m_marbleMap ) {
        return;
    }

    Marble::RouteRequest* request = d->m_marbleMap->model()->routingManager()->routeRequest();
    if ( index < request->size() ) {
        d->m_marbleMap->model()->routingManager()->routeRequest()->remove( index );
    }
}

void Routing::reverseRoute()
{
    if ( d->m_marbleMap ) {
        d->m_marbleMap->model()->routingManager()->reverseRoute();
    }
}

void Routing::clearRoute()
{
    if ( d->m_marbleMap ) {
        d->m_marbleMap->model()->routingManager()->clearRoute();
    }
}

void Routing::updateRoute()
{
    if ( d->m_marbleMap ) {
        d->m_marbleMap->model()->routingManager()->retrieveRoute();
    }
}

void Routing::openRoute( const QString &fileName )
{
    if ( d->m_marbleMap ) {
        Marble::RoutingManager * const routingManager = d->m_marbleMap->model()->routingManager();
        /** @todo FIXME: replace the file:// prefix on QML side */
        routingManager->clearRoute();
        QString target = fileName.startsWith( QLatin1String( "file://" ) ) ? fileName.mid( 7 ) : fileName;
        routingManager->loadRoute( target );
        Marble::GeoDataDocument* route = routingManager->alternativeRoutesModel()->currentRoute();
        if ( route ) {
            const Marble::GeoDataLineString* waypoints = Marble::AlternativeRoutesModel::waypoints( route );
            if ( waypoints ) {
                GeoDataCoordinates const center = waypoints->latLonAltBox().center();
                GeoDataCoordinates::Unit const inDegree = GeoDataCoordinates::Degree;
                d->m_marbleMap->centerOn( center.longitude(inDegree), center.latitude(inDegree) );
            }
        }
    }
}

void Routing::saveRoute( const QString &fileName )
{
    if ( d->m_marbleMap ) {
        /** @todo FIXME: replace the file:// prefix on QML side */
        QString target = fileName.startsWith( QLatin1String( "file://" ) ) ? fileName.mid( 7 ) : fileName;
        d->m_marbleMap->model()->routingManager()->saveRoute( target );
    }
}

}

#include "moc_Routing.cpp"
