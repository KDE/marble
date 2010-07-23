//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_ROUTINGMANAGER_H
#define MARBLE_ROUTINGMANAGER_H

#include "GeoDataCoordinates.h"

#include <QtCore/QAbstractItemModel>

namespace Marble
{

class RoutingManagerPrivate;
class MarbleWidget;
class RoutingModel;
class RouteSkeleton;
class GeoDataDocument;

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
    explicit RoutingManager( MarbleWidget *widget, QObject *parent = 0 );

    /** Destructor */
    ~RoutingManager();

    /**
      * Provides access to the routing model which contains a list
      * of routing instructions describing steps to get from the
      * source to the destination.
      * @see retrieveDirections
      */
    RoutingModel *routingModel();

    /**
      * Forward the source and destination locations to the current routing
      * provider to retrieve directions. If a route is retrieved, the direction
      * model will be updated with the routing instructions.
      * @param source Start location. Any string the routing provider supports as input.
      * @param destination Destination location. Any string the routing provider supports as input.
      * @see directionModel
      */
    void retrieveRoute( RouteSkeleton *route );

    /**
      * Toggle offline mode. In offline mode, new routes cannot be downloaded
      */
    void setWorkOffline( bool offline );

public Q_SLOTS:
    /** Update the route */
    void updateRoute();

Q_SIGNALS:
    /**
      * Directions and waypoints for the given route are being downloaded or have
      * been retrieved -- newState tells which of both
      */
    void stateChanged( RoutingManager::State newState, RouteSkeleton *route );

    void routeRetrieved( GeoDataDocument* route );

private Q_SLOTS:
    void retrieveRoute( GeoDataDocument* route );

private:
    friend class RoutingManagerPrivate;
    RoutingManagerPrivate *const d;
};

} // namespace Marble

#endif
