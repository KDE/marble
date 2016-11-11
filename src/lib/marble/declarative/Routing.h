//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DECLARATIVE_ROUTING_H
#define MARBLE_DECLARATIVE_ROUTING_H

#include <QQuickItem>

#include <Placemark.h>
#include <routing/RoutingModel.h>
#include <RouteRequestModel.h>

namespace Marble {

class MarbleMap;
class RoutingPrivate;

class Routing : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY( MarbleMap* marbleMap READ marbleMap WRITE setMarbleMap NOTIFY marbleMapChanged)
    Q_PROPERTY( QString routingProfile READ routingProfile WRITE setRoutingProfile NOTIFY routingProfileChanged )
    Q_PROPERTY( bool hasRoute READ hasRoute NOTIFY hasRouteChanged )
    Q_PROPERTY( RoutingModel* routingModel READ routingModel NOTIFY routingModelChanged)
    Q_PROPERTY( QQmlComponent* waypointDelegate READ waypointDelegate WRITE setWaypointDelegate NOTIFY waypointDelegateChanged)
    Q_PROPERTY( RouteRequestModel* routeRequestModel READ routeRequestModel NOTIFY routeRequestModelChanged)

public:
    enum RoutingProfile { Motorcar, Bicycle, Pedestrian };

    explicit Routing( QQuickItem* parent = 0 );

    ~Routing();

    void setMarbleMap( MarbleMap* marbleMap );

    MarbleMap *marbleMap();

    QString routingProfile() const;

    void setRoutingProfile( const QString & profile );

    bool hasRoute() const;

    RoutingModel *routingModel();

    QQmlComponent * waypointDelegate() const;

    Q_INVOKABLE int waypointCount() const;

    RouteRequestModel* routeRequestModel();

public Q_SLOTS:
    void addVia( qreal lon, qreal lat );

    void addViaAtIndex( int index, qreal lon, qreal lat );

    void addViaByPlacemark( Placemark * placemark );

    void addViaByPlacemarkAtIndex( int index, Placemark * placemark );

    void setVia( int index, qreal lon, qreal lat );

    void removeVia( int index );

    void swapVias( int index1, int index2 );

    void reverseRoute();

    void clearRoute();

    void updateRoute();

    void openRoute( const QString &filename );

    void saveRoute( const QString &filename );

    QObject* waypointModel();

    void setWaypointDelegate(QQmlComponent * waypointDelegate);

    int addSearchResultPlacemark( Placemark * placemark );

    void clearSearchResultPlacemarks();

Q_SIGNALS:
    void marbleMapChanged();

    void routingProfileChanged();

    void hasRouteChanged();

    void routingModelChanged();

    void waypointDelegateChanged(QQmlComponent * waypointDelegate);

    void routeRequestModelChanged(RouteRequestModel* routeRequestModel);

protected:
    // Implements QQuickItem interface
    QSGNode * updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

private Q_SLOTS:
    void updateWaypointItems();

    void updateSearchResultPlacemarks();

private:
    RoutingPrivate* const d;    
};

}

#endif
