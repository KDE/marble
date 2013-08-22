//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef CLOUDSYNCMANAGER_H
#define CLOUDSYNCMANAGER_H

#include <QObject>
#include <QUrl>

#include "RouteSyncManager.h"
#include "marble_export.h"
#include "routing/RoutingManager.h"

namespace Marble {

class MARBLE_EXPORT CloudSyncManager : public QObject
{
    Q_OBJECT
    
public:
    explicit CloudSyncManager( QObject *parent = 0 );
    ~CloudSyncManager();

    /**
     * Getter for offine mode.
     * @return true if offline mode enabled
     */
    bool workOffline();

    /**
     * Setter for offine mode.
     * @param offline Status of offline mode
     */
    void setWorkOffline( bool offline );

    enum Backend {
        Owncloud
    };

    /**
     * Checks if the user enabled synchronization.
     * @return true if synchronization enabled
     */
    bool isSyncEnabled();

    /**
     * Checks if the user enabled route synchronization.
     * @return true if route synchronization enabled
     */
    bool isRouteSyncEnabled();

    /**
     * Getter for currently selected backend.
     * @return Selected backend
     */
    Backend backend() const;

    /**
     * Gets ownCloud server from settings.
     * @return ownCloud server
     */
    QString server() const;

    /**
     * Gets ownCloud username from settings.
     * @return ownCloud username
     */
    QString username() const;

    /**
     * Gets ownCloud password from settings
     * @return ownCloud password
     */
    QString password() const;

    /**
     * Setter for enabling/disabling synchronization.
     * @param enabled Status of synchronization.
     */
    void setSyncEnabled( bool enabled );

    /**
     * Setter for enabling/disabling synchronization.
     * @param enabled Status of route synchronization
     */
    void setRouteSyncEnabled( bool enabled );

    /**
     * Setter for ownCloud server.
     * @param server ownCloud server
     */
    void setOwncloudServer( const QString &server );

    /**
     * Setter for ownCloud username.
     * @param username ownCloud username
     */
    void setOwncloudUsername( const QString &username );

    /**
     * Setter for ownCloud password.
     * @param password ownCloud password
     */
    void setOwncloudPassword( const QString &password );

    /**
     * Returns API path as a QString.
     * @return API path
     */
    QString apiPath() const;

    /**
     * Returns an API url ready for use.
     * @return API url as QString
     */
    QUrl apiUrl() const;

private:
    class Private;
    Private *d;
};

}

#endif // CLOUDSYNCMANAGER_H
