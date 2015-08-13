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
#include <declarative/RouteRequestModel.h>
#include <ViewportParams.h>

#include <QDebug>
#include <QPainter>
#include <QQmlContext>

namespace Marble {

class RoutingPrivate
{
public:
    RoutingPrivate(QObject * parent = nullptr);

    MarbleMap* m_marbleMap;
    QMap<QString, Marble::RoutingProfile> m_profiles;
    QString m_routingProfile;
    QQmlComponent * m_waypointDelegate;
    QMap<int,QQuickItem*> m_waypointItems;
    RouteRequestModel* m_routeRequestModel;
    QObject * m_parent;
};

RoutingPrivate::RoutingPrivate(QObject *parent) :
    m_marbleMap( nullptr ),
    m_waypointDelegate( nullptr ),
    m_routeRequestModel( new RouteRequestModel(parent) ),
    m_parent( parent )
{
    // nothing to do
}

Routing::Routing( QQuickItem *parent) :
    QQuickPaintedItem( parent ), d( new RoutingPrivate(this) )
{
    d->m_routeRequestModel->setRouting(this);
    connect(d->m_routeRequestModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(updateWaypointItems()));
    connect(d->m_routeRequestModel, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(updateWaypointItems()));
    connect(d->m_routeRequestModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(updateWaypointItems()));

    emit routeRequestModelChanged(d->m_routeRequestModel);
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

        int const dpi = qMax(paintDevice->logicalDpiX(), paintDevice->logicalDpiY());
        QPen standardRoutePen( routingManager->routeColorStandard().darker( 200 ) );
        qreal const width = 2.5 * MM2M * M2IN * dpi;
        standardRoutePen.setWidthF( width );
        geoPainter.setPen( standardRoutePen );
        geoPainter.drawPolyline( waypoints );

        standardRoutePen.setColor( routingManager->routeColorStandard() );
        standardRoutePen.setWidthF( width - 4.0 );
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

void Routing::setWaypointDelegate(QQmlComponent *waypointDelegate)
{
    if (d->m_waypointDelegate == waypointDelegate) {
        return;
    }

    d->m_waypointDelegate = waypointDelegate;
    emit waypointDelegateChanged(waypointDelegate);
}

void Routing::updateWaypointItems()
{
    if ( d->m_marbleMap && d->m_routeRequestModel ) {

        for (int i = d->m_waypointItems.keys().size(); i < d->m_routeRequestModel->rowCount(); i++ ) {
            QQmlContext * context = new QQmlContext( qmlContext( d->m_waypointDelegate ) );
            context->setContextProperty("type", QVariant::fromValue(QString("waypoint")));
            context->setContextProperty("visible", QVariant::fromValue(false));
            QObject * component = d->m_waypointDelegate->create(context);
            QQuickItem* item = qobject_cast<QQuickItem*>( component );
            if ( item ) {
                item->setParentItem( this );
                d->m_waypointItems[i] = item;
            } else {
                delete component;
            }
        }

        for (int i = d->m_waypointItems.keys().size()-1; i >= d->m_routeRequestModel->rowCount(); i--) {
            QQuickItem* item = d->m_waypointItems[i];
            item->setProperty("visible", QVariant::fromValue(false) );
            d->m_waypointItems.erase(d->m_waypointItems.find(i));
            item->deleteLater();
        }

        QMap<int, QQuickItem*>::iterator iter = d->m_waypointItems.begin();
        while ( iter != d->m_waypointItems.end() ) {
            qreal x = 0;
            qreal y = 0;
            const qreal lon = d->m_routeRequestModel->data(d->m_routeRequestModel->index( iter.key() ), RouteRequestModel::LongitudeRole).toFloat();
            const qreal lat = d->m_routeRequestModel->data(d->m_routeRequestModel->index( iter.key() ), RouteRequestModel::LatitudeRole).toFloat();
            const bool visible = d->m_marbleMap->viewport()->screenCoordinates(lon * DEG2RAD, lat * DEG2RAD, x, y);

            QQuickItem * item = iter.value();
            if ( item ) {
                item->setVisible( visible );
                if ( visible ) {
                    item->setX( x - item->width()/2 );
                    item->setY( y - item->height() );
                    if (iter.key() == d->m_waypointItems.keys().size()-1) {
                        item->setProperty("type", QVariant::fromValue(QString("destination")));
                    }
                    else if (iter.key() > 0) {
                        item->setProperty("type", QVariant::fromValue(QString("waypoint")));
                    }
                    else {
                        item->setProperty("type", QVariant::fromValue(QString("departure")));
                    }
                }
            }
            ++iter;
        }
    }
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

        connect( routingManager, SIGNAL(stateChanged(RoutingManager::State)), this, SLOT(update()));
        connect( routingManager, SIGNAL(stateChanged(RoutingManager::State)),
                 this, SIGNAL(hasRouteChanged()) );
        connect( routingModel(), SIGNAL(currentRouteChanged()),
                 this, SIGNAL(hasRouteChanged()) );
        connect( d->m_marbleMap, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)),
                 this, SLOT(updateWaypointItems()) );

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

QQmlComponent *Routing::waypointDelegate() const
{
    return d->m_waypointDelegate;
}

int Routing::waypointCount() const
{
    return d->m_routeRequestModel ? d->m_routeRequestModel->rowCount() : 0;
}

RouteRequestModel *Routing::routeRequestModel()
{
    return d->m_routeRequestModel;
}

void Routing::addVia( qreal lon, qreal lat )
{
    if ( d->m_marbleMap ) {
        Marble::RouteRequest* request = d->m_marbleMap->model()->routingManager()->routeRequest();
        request->append( Marble::GeoDataCoordinates( lon, lat, 0.0, Marble::GeoDataCoordinates::Degree ) );
        updateRoute();
    }
}

void Routing::addViaAtIndex(int index, qreal lon, qreal lat)
{
    if ( d->m_marbleMap ) {
        Marble::RouteRequest * request = d->m_marbleMap->model()->routingManager()->routeRequest();
        request->insert(index, Marble::GeoDataCoordinates( lon, lat, 0.0, Marble::GeoDataCoordinates::Degree) );
        updateRoute();
    }
}

void Routing::addViaByCoordinate(Coordinate *coordinate)
{
    addVia(coordinate->longitude(), coordinate->latitude());
}

void Routing::addViaByCoordinateAtIndex(int index, Coordinate *coordinate)
{
    addViaAtIndex(index, coordinate->longitude(), coordinate->latitude());
}

void Routing::addViaByPlacemark(Placemark *placemark)
{
    if ( d->m_marbleMap ) {
        Marble::RouteRequest * request = d->m_marbleMap->model()->routingManager()->routeRequest();
        request->append(placemark->coordinate()->coordinates(), placemark->name());
        updateRoute();
    }
}

void Routing::addViaByPlacemarkAtIndex(int index, Placemark *placemark)
{
    if ( d->m_marbleMap ) {
        Marble::RouteRequest * request = d->m_marbleMap->model()->routingManager()->routeRequest();
        request->insert(index, placemark->coordinate()->coordinates(), placemark->name());
        updateRoute();
    }
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

    updateRoute();
}

void Routing::swapVias(int index1, int index2)
{
    if ( !d->m_marbleMap || !d->m_routeRequestModel ) {
        return;
    }

    Marble::RouteRequest* request = d->m_marbleMap->model()->routingManager()->routeRequest();
    request->swap(index1, index2);
    updateRoute();
    updateWaypointItems();
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
