// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DECLARATIVE_ROUTING_H
#define MARBLE_DECLARATIVE_ROUTING_H

#include "routing/AlternativeRoutesModel.h"
#include <QQuickItem>

#include <Placemark.h>
#include <RouteRequestModel.h>
#include <routing/RoutingModel.h>

namespace Marble
{

class MarbleMap;
class RoutingPrivate;

class Routing : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(MarbleMap *marbleMap READ marbleMap WRITE setMarbleMap NOTIFY marbleMapChanged)
    Q_PROPERTY(RoutingProfile routingProfile READ routingProfile WRITE setRoutingProfile NOTIFY routingProfileChanged)
    Q_PROPERTY(bool hasRoute READ hasRoute NOTIFY hasRouteChanged)
    Q_PROPERTY(bool hasWaypoints READ hasWaypoints NOTIFY hasWaypointsChanged)
    Q_PROPERTY(RoutingModel *routingModel READ routingModel NOTIFY routingModelChanged)
    Q_PROPERTY(QQmlComponent *waypointDelegate READ waypointDelegate WRITE setWaypointDelegate NOTIFY waypointDelegateChanged)
    Q_PROPERTY(RouteRequestModel *routeRequestModel READ routeRequestModel CONSTANT)
    Q_PROPERTY(AlternativeRoutesModel *alternativeRoutesModel READ alternativeRoutesModel NOTIFY marbleMapChanged)

public:
    enum RoutingProfile {
        Motorcar,
        Bicycle,
        Pedestrian,
    };
    Q_ENUM(RoutingProfile)

    explicit Routing(QQuickItem *parent = nullptr);

    ~Routing() override;

    void setMarbleMap(MarbleMap *marbleMap);

    MarbleMap *marbleMap();

    RoutingProfile routingProfile() const;

    void setRoutingProfile(RoutingProfile profile);

    bool hasRoute() const;

    bool hasWaypoints() const;

    RoutingModel *routingModel();

    QQmlComponent *waypointDelegate() const;

    Q_INVOKABLE int waypointCount() const;

    RouteRequestModel *routeRequestModel() const;

    AlternativeRoutesModel *alternativeRoutesModel() const;

public Q_SLOTS:
    void addVia(qreal lon, qreal lat);

    void addViaAtIndex(int index, qreal lon, qreal lat);

    void addViaByPlacemark(Placemark *placemark);

    void addViaByPlacemarkAtIndex(int index, Placemark *placemark);

    void setVia(int index, qreal lon, qreal lat);

    void removeVia(int index);

    void swapVias(int index1, int index2);

    void reverseRoute();

    void clearRoute();

    void updateRoute();

    void openRoute(const QString &filename);

    void saveRoute(const QString &filename);

    QObject *waypointModel();

    void setWaypointDelegate(QQmlComponent *waypointDelegate);

    int addSearchResultPlacemark(Placemark *placemark);

    void clearSearchResultPlacemarks();

Q_SIGNALS:
    void marbleMapChanged();

    void routingProfileChanged();

    void hasRouteChanged();

    void hasWaypointsChanged();

    void routingModelChanged();

    void waypointDelegateChanged(QQmlComponent *waypointDelegate);

    void routeRequestModelChanged(RouteRequestModel *routeRequestModel);

protected:
    // Implements QQuickItem interface
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private Q_SLOTS:
    void updateWaypointItems();

    void updateSearchResultPlacemarks();

private:
    RoutingPrivate *const d;
};

}

#endif
