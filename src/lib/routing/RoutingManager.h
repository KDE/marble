/*
    Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MARBLE_ROUTING_MANAGER_H
#define MARBLE_ROUTING_MANAGER_H

#include "GeoDataCoordinates.h"
#include "routing/AbstractRoutingProvider.h"

#include <QtCore/QAbstractItemModel>

namespace Marble {

class RoutingManagerPrivate;
class MarbleWidget;
class RoutingModel;

/**
  * Delegates data retrieval and model updates to the appropriate
  * routing provider -- always openrouteservice.org for now.
  */
class RoutingManager : public QObject
{
    Q_OBJECT

public:
    enum State {
      Downloading, // A new route is downloaded in the background
      Retrieved // No download in progress
    };

    /** Constructor */
    RoutingManager(MarbleWidget *widget, QObject *parent = 0);

    /** Destructor */
    ~RoutingManager();

    /**
      * Provides access to the routing model which contains a list
      * of routing instructions describing steps to get from the
      * source to the destination.
      * @see retrieveDirections
      */
    RoutingModel* routingModel();

    /**
      * Forward the source and destination locations to the current routing
      * provider to retrieve directions. If a route is retrieved, the direction
      * model will be updated with the routing instructions.
      * @param source Start location. Any string the routing provider supports as input.
      * @param destination Destination location. Any string the routing provider supports as input.
      * @see directionModel
      */
    void retrieveRoute(const GeoDataLineString &route);

Q_SIGNALS:
    /**
      * Directions and waypoints for the given route are being downloaded or have
      * been retrieved -- newState tells which of both
      */
    void stateChanged(RoutingManager::State newState, const GeoDataLineString &route);

private Q_SLOTS:
    /** Routing provider has finished downloading data */
    void setRouteData(AbstractRoutingProvider::Format format, const QByteArray &data);

    /** Model was changed (start/destination moved), update route */
    void scheduleRouteUpdate();

    /** Update the route */
    void updateRoute();

private:
    RoutingManagerPrivate* const d;
};

} // namespace Marble

#endif // MARBLE_ROUTING_MANAGER_H
