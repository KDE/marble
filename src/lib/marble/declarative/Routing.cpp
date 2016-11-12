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

#include <MarbleMap.h>
#include <MarbleModel.h>
#include "MarbleDirs.h"
#include "routing/AlternativeRoutesModel.h"
#include "routing/RoutingManager.h"
#include "routing/RouteRequest.h"
#include "routing/RoutingProfilesModel.h"
#include <GeoDataLatLonAltBox.h>
#include <GeoPainter.h>
#include <routing/Route.h>
#include <declarative/RouteRequestModel.h>
#include <ViewportParams.h>
#include <PositionTracking.h>

#include <QDebug>
#include <QQmlContext>
#include <QOpenGLPaintDevice>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

namespace Marble {

class RoutingPrivate
{
public:
    explicit RoutingPrivate(QObject * parent = nullptr);

    MarbleMap* m_marbleMap;
    QMap<QString, Marble::RoutingProfile> m_profiles;
    QString m_routingProfile;
    QQmlComponent * m_waypointDelegate;
    QMap<int,QQuickItem*> m_waypointItems;
    RouteRequestModel* m_routeRequestModel;
    QObject * m_parent;
    QVector<Placemark *> m_searchResultPlacemarks;
    QMap<int, QQuickItem*> m_searchResultItems;
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
    QQuickItem( parent ), d( new RoutingPrivate(this) )
{
    setFlag(ItemHasContents, true);
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

QSGNode * Routing::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) {
    if (!d->m_marbleMap) {
        return 0;
    }

    QOpenGLPaintDevice paintDevice(QSize(width(), height()));
    Marble::GeoPainter geoPainter(&paintDevice, d->m_marbleMap->viewport(), d->m_marbleMap->mapQuality());

    RoutingManager const * const routingManager = d->m_marbleMap->model()->routingManager();
    GeoDataLineString const & waypoints = routingManager->routingModel()->route().path();

    if (waypoints.isEmpty()) {
      return 0;
    }

    int const dpi = qMax(paintDevice.logicalDpiX(), paintDevice.logicalDpiY());
    qreal const width = 2.5 * MM2M * M2IN * dpi;

    QColor standardRouteColor = routingManager->state() == RoutingManager::Downloading ?
                                routingManager->routeColorStandard() :
                                routingManager->routeColorStandard().darker( 200 );

    QVector<QPolygonF*> polygons;
    geoPainter.polygonsFromLineString( waypoints, polygons);

    if (!polygons.isEmpty()) {
        delete oldNode;
        oldNode = new QSGNode;
        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(standardRouteColor);

        foreach(const QPolygonF* itPolygon, polygons) {

            int segmentCount = itPolygon->size() - 1;

            QSGGeometryNode * lineNode = new QSGGeometryNode;

            QSGGeometry * lineNodeGeo = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 2*segmentCount);
            lineNodeGeo->setLineWidth(width);
            lineNodeGeo->setDrawingMode(GL_LINE_STRIP);
            lineNodeGeo->setLineWidth(width);
            lineNodeGeo->allocate(2*segmentCount);

            lineNode->setGeometry(lineNodeGeo);
            lineNode->setFlag(QSGNode::OwnsGeometry);
            lineNode->setMaterial(material);
            lineNode->setFlag(QSGNode::OwnsMaterial);

            for(int i = 0; i < segmentCount; ++i) {
                lineNodeGeo->vertexDataAsPoint2D()[2*i].set(itPolygon->at(i).x(), itPolygon->at(i).y());
                lineNodeGeo->vertexDataAsPoint2D()[2*i+1].set(itPolygon->at(i+1).x(), itPolygon->at(i+1).y());
            }
            oldNode->appendChildNode(lineNode);
        }
    }

    qDeleteAll(polygons);
    return oldNode;
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
            QObject * component = d->m_waypointDelegate->create(context);
            QQuickItem* item = qobject_cast<QQuickItem*>( component );
            if ( item ) {
                item->setParentItem( this );
                item->setProperty("index", i);
                d->m_waypointItems[i] = item;
            } else {
                delete component;
            }
        }

        for (int i = d->m_waypointItems.keys().size()-1; i >= d->m_routeRequestModel->rowCount(); i--) {
            QQuickItem* item = d->m_waypointItems[i];
            item->setProperty("visible", QVariant(false) );
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
                    item->setProperty("xPos", QVariant(x));
                    item->setProperty("yPos", QVariant(y));
                    if (iter.key() == 0 && waypointCount() == 1) {
                        item->setProperty("type", QVariant(QStringLiteral("departure")));
                    }
                    else if (iter.key() == d->m_waypointItems.keys().size()-1) {
                        item->setProperty("type", QVariant(QStringLiteral("destination")));
                    }
                    else if (iter.key() > 0) {
                        item->setProperty("type", QVariant(QStringLiteral("waypoint")));
                    }
                    else {
                        item->setProperty("type", QVariant(QStringLiteral("departure")));
                    }
                }
            }
            ++iter;
        }
    }
}

int Routing::addSearchResultPlacemark(Placemark *placemark)
{
    if ( d->m_marbleMap ) {
        for (int i = 0; i < d->m_searchResultItems.size(); i++) {
            if (d->m_searchResultPlacemarks[i]->placemark().coordinate() == placemark->placemark().coordinate()) {
                return i;
            }
        }
        Placemark * newPlacemark = new Placemark(this);
        newPlacemark->setGeoDataPlacemark(placemark->placemark());
        d->m_searchResultPlacemarks.push_back(newPlacemark);
    }

    updateSearchResultPlacemarks();
    return d->m_searchResultPlacemarks.size()-1;
}

void Routing::clearSearchResultPlacemarks()
{
    foreach(Placemark* placemark, d->m_searchResultPlacemarks) {
        placemark->deleteLater();
    }
    d->m_searchResultPlacemarks.clear();

    foreach(QQuickItem* item, d->m_searchResultItems) {
        item->deleteLater();
    }
    d->m_searchResultItems.clear();
}

void Routing::updateSearchResultPlacemarks()
{
    for (int i = d->m_searchResultItems.keys().size(); i < d->m_searchResultPlacemarks.size(); i++ ) {
        QQmlContext * context = new QQmlContext( qmlContext( d->m_waypointDelegate ) );
        QObject * component = d->m_waypointDelegate->create(context);
        QQuickItem* item = qobject_cast<QQuickItem*>( component );
        if ( item ) {
            item->setParentItem( this );
            item->setProperty("index", i);
            item->setProperty("type", QVariant(QStringLiteral("searchResult")));
            item->setProperty("placemark", QVariant::fromValue(d->m_searchResultPlacemarks[i]));
            d->m_searchResultItems[i] = item;
        } else {
            delete component;
        }
    }

    for (int i = d->m_searchResultItems.keys().size()-1; i >= d->m_searchResultPlacemarks.size(); i--) {
        QQuickItem* item = d->m_searchResultItems[i];
        item->setProperty("visible", QVariant(false) );
        d->m_searchResultItems.erase(d->m_searchResultItems.find(i));
        item->deleteLater();
    }

    for (int i = 0; i < d->m_searchResultItems.keys().size() && i < d->m_searchResultPlacemarks.size(); i++) {
        qreal x = 0;
        qreal y = 0;
        const qreal lon = d->m_searchResultPlacemarks[i]->placemark().coordinate().longitude();
        const qreal lat = d->m_searchResultPlacemarks[i]->placemark().coordinate().latitude();
        const bool visible = d->m_marbleMap->viewport()->screenCoordinates(lon, lat, x, y);

        QQuickItem * item = d->m_searchResultItems[i];
        if ( item ) {
            item->setVisible( visible );
            if ( visible ) {
                item->setProperty("xPos", QVariant(x));
                item->setProperty("yPos", QVariant(y));
            }
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
        connect( routingManager, SIGNAL(routeRetrieved(GeoDataDocument*)), this, SLOT(update()));
        connect( routingManager, SIGNAL(stateChanged(RoutingManager::State)),
                 this, SIGNAL(hasRouteChanged()) );
        connect( routingModel(), SIGNAL(currentRouteChanged()),
                 this, SIGNAL(hasRouteChanged()) );
        connect( routingModel(), SIGNAL(currentRouteChanged()),
                 this, SLOT(update()) );
        connect( d->m_marbleMap, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)),
                 this, SLOT(updateWaypointItems()) );
        connect( d->m_marbleMap, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)),
                 this, SLOT(updateSearchResultPlacemarks()) );

        emit routingModelChanged();

        QList<Marble::RoutingProfile> profiles = routingManager->profilesModel()->profiles();
        if ( profiles.size() == 4 ) {
            /** @todo FIXME: Restrictive assumptions on available plugins and certain profile loading implementation */
            d->m_profiles[QStringLiteral("Motorcar")] = profiles.at( 0 );
            d->m_profiles[QStringLiteral("Bicycle")] = profiles.at( 2 );
            d->m_profiles[QStringLiteral("Pedestrian")] = profiles.at( 3 );
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
        request->addVia( Marble::GeoDataCoordinates( lon, lat, 0.0, Marble::GeoDataCoordinates::Degree ) );
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

void Routing::addViaByPlacemark(Placemark *placemark)
{
    if (d->m_marbleMap && placemark) {
        Marble::RouteRequest * request = d->m_marbleMap->model()->routingManager()->routeRequest();
        request->addVia(placemark->placemark());
        updateRoute();
    }
}

void Routing::addViaByPlacemarkAtIndex(int index, Placemark *placemark)
{
    if (d->m_marbleMap && placemark) {
        Marble::RouteRequest * request = d->m_marbleMap->model()->routingManager()->routeRequest();
        request->insert(index, placemark->placemark());
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
