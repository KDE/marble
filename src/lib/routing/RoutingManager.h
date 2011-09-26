//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_ROUTINGMANAGER_H
#define MARBLE_ROUTINGMANAGER_H

#include "marble_export.h"
#include "GeoDataCoordinates.h"

#include <QtCore/QAbstractItemModel>

namespace Marble
{

class RoutingManagerPrivate;
class RoutingModel;
class RouteRequest;
class MarbleModel;
class GeoDataDocument;
class GeoDataFolder;
class AlternativeRoutesModel;
class AdjustNavigation;
class RoutingProfilesModel;

/**
  * Delegates data retrieval and model updates to the appropriate
  * routing provider -- always openrouteservice.org for now.
  */
class MARBLE_EXPORT RoutingManager : public QObject
{
    Q_OBJECT

public:
    enum State {
        Downloading, // A new route is downloaded in the background
        Retrieved // No download in progress
    };

    /** Constructor */
    explicit RoutingManager( MarbleModel *marbleModel, QObject *parent = 0 );

    /** Destructor */
    ~RoutingManager();

    /**
      * Provides access to the model which contains all possible routing profiles
      */
    RoutingProfilesModel *profilesModel();

    /**
      * Provides access to the routing model which contains a list
      * of routing instructions describing steps to get from the
      * source to the destination.
      * @see retrieveDirections
      */
    RoutingModel *routingModel();

    /**
      * Provides access to the model which contains a list of
      * alternative routes
      */
    AlternativeRoutesModel* alternativeRoutesModel();

    // This is a temporary location for AdjustNavigation. The code would be refactored soon.
    void setAdjustNavigation( AdjustNavigation * adjustNavigation );

    /**
     * returns the instance of AdjustNavigation
     * This is a temporary location for AdjustNavigation. The code would be refactored soon.
     */
    AdjustNavigation* adjustNavigation();

    /**
      * Returns the current route request
      */
    RouteRequest* routeRequest();

    /**
      * Saves the current route request and the current route to disk
      */
    void writeSettings() const;

    /**
      * Restores a previously saved route request and route from disk, if any
      */
    void readSettings();

    /**
      * Saves the current route to the file with the given filename. Existing files
      * will be overwritten. The route is saved in kml format.
      */
    void saveRoute( const QString &filename ) const;

    /**
      * Opens the given filename (kml format) and loads the route contained in it
      */
    void loadRoute( const QString &filename );

    /**
      * Set whether a warning message should be shown to the user before
      * starting guidance mode.
      */
    void setShowGuidanceModeStartupWarning( bool show );

    /**
      * Returns true (default) if a warning is shown to the user when starting guidance
      * mode.
      */
    bool showGuidanceModeStartupWarning() const;

public Q_SLOTS:
    /** Reverse the previously requested route, i.e. swap start and destination (and via points, if any) */
    void reverseRoute();

    /** Update the route */
    void updateRoute();

    /** Clear all via points */
    void clearRoute();

    /** Toggle turn by turn navigation mode */
    void setGuidanceModeEnabled( bool enabled );

Q_SIGNALS:
    /**
      * Directions and waypoints for the given route are being downloaded or have
      * been retrieved -- newState tells which of both
      */
    void stateChanged( RoutingManager::State newState, RouteRequest *route );

    void routeRetrieved( GeoDataDocument* route );

private:
    Q_PRIVATE_SLOT( d, void addRoute( GeoDataDocument* route ) )

    Q_PRIVATE_SLOT( d, void recalculateRoute( bool deviated ) )

private:
    friend class RoutingManagerPrivate;
    RoutingManagerPrivate *const d;
};

} // namespace Marble

#endif
