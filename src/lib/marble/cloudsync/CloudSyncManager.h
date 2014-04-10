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

#include "marble_export.h"

namespace Marble {

class BookmarkSyncManager;
class RouteSyncManager;

class MARBLE_EXPORT CloudSyncManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool workOffline READ workOffline WRITE setWorkOffline NOTIFY workOfflineChanged )
    Q_PROPERTY( bool syncEnabled READ isSyncEnabled WRITE setSyncEnabled NOTIFY syncEnabledChanged )

    Q_PROPERTY( QString owncloudUsername READ owncloudUsername WRITE setOwncloudUsername NOTIFY owncloudUsernameChanged )
    Q_PROPERTY( QString owncloudPassword READ owncloudPassword WRITE setOwncloudPassword NOTIFY owncloudPasswordChanged )
    Q_PROPERTY( QString owncloudServer READ owncloudServer WRITE setOwncloudServer NOTIFY owncloudServerChanged )
    Q_PROPERTY( QUrl apiUrl READ apiUrl NOTIFY apiUrlChanged )

    Q_PROPERTY( BookmarkSyncManager* bookmarkSyncManager READ bookmarkSyncManager NOTIFY bookmarkSyncManagerChanged )
    Q_PROPERTY( RouteSyncManager* routeSyncManager READ routeSyncManager NOTIFY routeSyncManagerChanged )

public:
    explicit CloudSyncManager( QObject *parent = 0 );
    ~CloudSyncManager();

    /**
     * Status enumeration
     */
    enum Status { Unknown, Success, Error };

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

    /**
     * Checks if the user enabled synchronization.
     * @return true if synchronization enabled
     */
    bool isSyncEnabled() const;

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
     * Change owncloud server URL, username and password at once without emitting change
     * signals like setOwncloudServer, setOwncloudUsername, setOwncloudPassword do.
     * The apiUrlChanged signal is emitted, if needed, however.
     * @brief setOwncloudCredentials
     * @param server
     * @param user
     * @param password
     */
    void setOwncloudCredentials( const QString &server, const QString &user, const QString &password );

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
     * Setter for authentication error.
     * @param error authentication error
     */
    void setStatus( const QString &status, CloudSyncManager::Status status_type );

    /**
     * @return Last synchronization status
     */
    CloudSyncManager::Status status() const;

    /**
     * @return A human readable description of the last synchronization status
     */
    QString statusDescription() const;

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

    /**
     * @return A ready to use BookmarkSyncManager instance
     */
    RouteSyncManager* routeSyncManager();

    /**
     * @return A ready to use BookmarkSyncManager instance
     */
    BookmarkSyncManager* bookmarkSyncManager();

Q_SIGNALS:
    void workOfflineChanged(bool workOffline);
    void syncEnabledChanged(bool enabled);

    void owncloudUsernameChanged(const QString &username);
    void owncloudPasswordChanged(const QString &password);
    void owncloudServerChanged(const QString &server);
    void statusChanged(const QString &status);

    void apiUrlChanged(const QUrl &url);
    void routeSyncManagerChanged();
    void bookmarkSyncManagerChanged();

private:
    class Private;
    Private *d;
};

}

#endif // CLOUDSYNCMANAGER_H
