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

    Q_PROPERTY( bool workOffline READ workOffline WRITE setWorkOffline NOTIFY workOfflineChanged )
    Q_PROPERTY( bool syncEnabled READ isSyncEnabled WRITE setSyncEnabled NOTIFY syncEnabledChanged )
    Q_PROPERTY( bool routeSyncEnabled READ isRouteSyncEnabled WRITE setRouteSyncEnabled NOTIFY routeSyncEnabledChanged )
    Q_PROPERTY( bool bookmarkSyncEnabled READ isBookmarkSyncEnabled WRITE setBookmarkSyncEnabled NOTIFY bookmarkSyncEnabledChanged )

    Q_PROPERTY( QString owncloudUsername READ owncloudUsername WRITE setOwncloudUsername NOTIFY owncloudUsernameChanged )
    Q_PROPERTY( QString owncloudPassword READ owncloudPassword WRITE setOwncloudPassword NOTIFY owncloudPasswordChanged )
    Q_PROPERTY( QString owncloudServer READ owncloudServer WRITE setOwncloudServer NOTIFY owncloudServerChanged )
    Q_PROPERTY( QUrl apiUrl READ apiUrl NOTIFY apiUrlChanged )

public:
    explicit CloudSyncManager( QObject *parent = 0 );
    ~CloudSyncManager();

    /**
     * Getter for offine mode.
     * @return true if offline mode enabled
     */
    bool workOffline() const;

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
    bool isSyncEnabled() const;

    /**
     * Checks if the user enabled route synchronization.
     * @return true if route synchronization enabled
     */
    bool isRouteSyncEnabled() const;

    /**
     * Checks if the user enabled bookmark synchronization.
     * @return true if bookmark synchronization enabled
     */
    bool isBookmarkSyncEnabled() const;

    /**
     * Getter for currently selected backend.
     * @return Selected backend
     */
    Backend backend() const;

    /**
     * Gets ownCloud server from settings.
     * @return ownCloud server
     */
    QString owncloudServer() const;

    /**
     * Gets ownCloud username from settings.
     * @return ownCloud username
     */
    QString owncloudUsername() const;

    /**
     * Gets ownCloud password from settings
     * @return ownCloud password
     */
    QString owncloudPassword() const;

    /**
     * Setter for enabling/disabling synchronization.
     * @param enabled Status of synchronization.
     */
    void setSyncEnabled( bool enabled );

    /**
     * Setter for enabling/disabling route synchronization.
     * @param enabled Status of route synchronization
     */
    void setRouteSyncEnabled( bool enabled );

    /**
     * Setter for enabling/disabling bookmark synchronization.
     * @param enabled Status of bookmark synchronization
     */
    void setBookmarkSyncEnabled( bool enabled );

    /**
     * Setter for ownCloud server.
     * @param server ownCloud server
     */
    void setOwncloudServer( const QString &owncloudServer );

    /**
     * Setter for ownCloud username.
     * @param username ownCloud username
     */
    void setOwncloudUsername( const QString &owncloudUsername );

    /**
     * Setter for ownCloud password.
     * @param password ownCloud password
     */
    void setOwncloudPassword( const QString &owncloudPassword );

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

Q_SIGNALS:
    void workOfflineChanged(bool workOffline);
    void syncEnabledChanged(bool enabled);
    void routeSyncEnabledChanged(bool enabled);
    void bookmarkSyncEnabledChanged(bool enabled);

    void owncloudUsernameChanged(const QString &username);
    void owncloudPasswordChanged(const QString &password);
    void owncloudServerChanged(const QString &server);

    void apiUrlChanged(const QUrl &url);

private:
    class Private;
    Private *d;
};

}

#endif // CLOUDSYNCMANAGER_H
