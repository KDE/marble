// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_ROUTINGMANAGER_H
#define MARBLE_ROUTINGMANAGER_H

#include "marble_export.h"
#include "RoutingProfile.h"

namespace Marble
{

class RoutingManagerPrivate;
class RoutingModel;
class RouteRequest;
class MarbleModel;
class GeoDataDocument;
class AlternativeRoutesModel;
class RoutingProfilesModel;

/**
  * Delegates data retrieval and model updates to the appropriate
  * routing provider.
  */
class MARBLE_EXPORT RoutingManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY( State state READ state NOTIFY stateChanged )
    Q_PROPERTY( bool guidanceModeEnabled READ guidanceModeEnabled WRITE setGuidanceModeEnabled NOTIFY guidanceModeEnabledChanged )

public:
    enum State {
        Downloading, // A new route is downloaded in the background
        Retrieved // No download in progress
    };

    /** Constructor */
    explicit RoutingManager( MarbleModel *marbleModel, QObject *parent = nullptr );

    /** Destructor */
    ~RoutingManager() override;

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

    const RoutingModel *routingModel() const;

    /**
      * Provides access to the model which contains a list of
      * alternative routes
      */
    AlternativeRoutesModel* alternativeRoutesModel();

    /**
      * Returns the current route request
      */
    RouteRequest* routeRequest();

    /**
     * @brief Returns whether a route is being downloaded
     * @return
     */
    State state() const;

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
      * Generates a routing profile with default settings for the given transport type
      */
    RoutingProfile defaultProfile( RoutingProfile::TransportType transportType ) const;

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

    /**
     * Set last directory the user opened a route from.
     */
    void setLastOpenPath( const QString &path );

    /**
     * Return last directory the user opened a route from.
     */
    QString lastOpenPath() const;

    /**
     * Set last directory the user saved a route to.
     */
    void setLastSavePath( const QString &path );

    /**
     * Return last directory the user saved a route to.
     */
    QString lastSavePath() const;

    /**
     * Set color for standard route rendering
     */
    void setRouteColorStandard( const QColor& color );

    /**
     * Get color for standard route rendering
     */
    QColor routeColorStandard() const;

    /**
     * Set color for highlighted route rendering
     */
    void setRouteColorHighlighted( const QColor& color );

    /**
     * Get color for highlighted route rendering
     */
    QColor routeColorHighlighted() const;

    /**
     * Set color for alternative route rendering
     */
    void setRouteColorAlternative( const QColor& color );

    /**
     * Get color for alternative route rendering
     */
    QColor routeColorAlternative() const;

    bool guidanceModeEnabled() const;

public Q_SLOTS:
    /** Reverse the previously requested route, i.e. swap start and destination (and via points, if any) */
    void reverseRoute();

    /** Retrieve a route suiting the routeRequest */
    void retrieveRoute();

    /** Clear all via points */
    void clearRoute();

    /** Toggle turn by turn navigation mode */
    void setGuidanceModeEnabled( bool enabled );

Q_SIGNALS:
    /**
      * Directions and waypoints for the given route are being downloaded or have
      * been retrieved -- newState tells which of both
      */
    void stateChanged( RoutingManager::State newState );

    void routeRetrieved( GeoDataDocument* route );

    void guidanceModeEnabledChanged( bool enabled );

private:
    Q_PRIVATE_SLOT( d, void addRoute( GeoDataDocument* route ) )

    Q_PRIVATE_SLOT( d, void routingFinished() )

    Q_PRIVATE_SLOT(d, void setCurrentRoute(const GeoDataDocument *route))

    Q_PRIVATE_SLOT( d, void recalculateRoute( bool deviated ) )

private:
    friend class RoutingManagerPrivate;
    RoutingManagerPrivate *const d;
};

} // namespace Marble

#endif
