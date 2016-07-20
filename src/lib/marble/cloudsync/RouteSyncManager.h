//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef ROUTESYNCMANAGER_H
#define ROUTESYNCMANAGER_H

#include "marble_export.h"

#include <QObject>

namespace Marble {

class CloudSyncManager;
class RoutingManager;
class CloudRouteModel;
class RouteItem;

class MARBLE_EXPORT RouteSyncManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool routeSyncEnabled READ isRouteSyncEnabled WRITE setRouteSyncEnabled NOTIFY routeSyncEnabledChanged )
    
public:
    explicit RouteSyncManager( CloudSyncManager *cloudSyncManager );
    ~RouteSyncManager();

    void setRoutingManager( RoutingManager *routingManager );

    /**
     * Checks if the user enabled route synchronization.
     * @return true if route synchronization enabled
     */
    bool isRouteSyncEnabled() const;

    /**
     * Setter for enabling/disabling route synchronization.
     * @param enabled Status of route synchronization
     */
    void setRouteSyncEnabled( bool enabled );

    /**
     * Returns CloudRouteModel associated with RouteSyncManager instance
     * @return CloudRouteModel associated with RouteSyncManager instance
     */
    CloudRouteModel *model();

    /**
     * Generates a timestamp which will be used as an unique identifier.
     * @return A timestamp.
     */
    QString generateTimestamp() const;

    /**
     * Saves the route displayed in Marble's routing widget to local cache directory.
     * Uses the RoutingManager passed as a parameter to the constructor.
     * @return Filename of saved file.
     */
    QString saveDisplayedToCache() const;

    /**
     * Uploads currently displayed route to cloud.
     * Initiates necessary methods of backends.
     * Note that, this also runs saveDisplayedToCache() method.
     */
    void uploadRoute();

    /**
     * Gathers data from local cache directory and returns a route list.
     * @return Routes stored in local cache
     */
    QVector<RouteItem> cachedRouteList() const;

public Q_SLOTS:
    /**
     * Uploads the route with given timestamp.
     * @param timestamp Timestamp of the route which will be uploaded.
     */
    void uploadRoute( const QString &timestamp );

    /**
     * Starts preparing a route list by downloading
     * a list of the routes on the cloud and adding
     * the ones on the
     */
    void prepareRouteList();

    /**
     * Starts the download of specified route.
     * @param timestamp Timestamp of the route that will be downloaded.
     * @see RouteSyncManager::saveDownloadedToCache()
     */
    void downloadRoute( const QString &timestamp );

    /**
     * Opens route.
     * @param timestamp Timestamp of the route that will be opened.
     */
    void openRoute( const QString &timestamp );

    /**
     * Deletes route from cloud.
     * @param timestamp Timestamp of the route that will be deleted.
     */
    void deleteRoute( const QString &timestamp );

    /**
     * Removes route from cache.
     * @param timestamp Timestamp of the route that will be removed.
     */
    void removeRouteFromCache( const QString &timestamp );

    /**
     * Updates upload progressbar.
     * @param sent Bytes sent.
     * @param total Total bytes.
     */
    void updateUploadProgressbar( qint64 sent, qint64 total );

private Q_SLOTS:
    /**
     * Appends downloaded route list to RouteSyncManager's private list
     * and then forwards the list to CloudRouteModel
     * @param routeList Downloaded route list
     */
    void setRouteModelItems( const QVector<RouteItem> &routeList );


Q_SIGNALS:
    void routeSyncEnabledChanged(bool enabled);
    void routeListDownloadProgress( qint64 received, qint64 total );
    void routeUploadProgress( qint64 sent, qint64 total );

private:
    class Private;
    Private *d;
};

}

#endif // ROUTESYNCMANAGER_H
