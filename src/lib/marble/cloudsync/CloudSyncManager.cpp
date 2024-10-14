// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Utku Aydın <utkuaydin34@gmail.com>
//

#include "CloudSyncManager.h"
#include "BookmarkSyncManager.h"
#include "RouteSyncManager.h"

namespace Marble
{

class Q_DECL_HIDDEN CloudSyncManager::Private
{
public:
    Private();
    ~Private();

    bool m_workOffline;

    bool m_syncEnabled;

    QString m_owncloudProtocol;
    QString m_ownloudServer;
    QString m_owncloudUsername;
    QString m_owncloudPassword;
    QString m_status;
    CloudSyncManager::Status m_status_type;

    RouteSyncManager *m_routeSyncManager = nullptr;
    BookmarkSyncManager *m_bookmarkSyncManager = nullptr;
};

CloudSyncManager::Private::~Private()
{
    delete m_routeSyncManager;
    delete m_bookmarkSyncManager;
}

CloudSyncManager::Private::Private()
    : m_workOffline(false)
    , m_syncEnabled(false)
    , m_owncloudProtocol(QStringLiteral("http"))
    , m_ownloudServer()
    , m_owncloudUsername()
    , m_owncloudPassword()
    , m_status()
    , m_status_type(CloudSyncManager::Unknown)
{
}

CloudSyncManager::CloudSyncManager(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
}

CloudSyncManager::~CloudSyncManager()
{
    delete d;
}

bool CloudSyncManager::workOffline() const
{
    return d->m_workOffline;
}

void CloudSyncManager::setWorkOffline(bool offline)
{
    if (offline != d->m_workOffline) {
        d->m_workOffline = offline;
        Q_EMIT workOfflineChanged(d->m_workOffline);
    }
}

bool CloudSyncManager::isSyncEnabled() const
{
    return d->m_syncEnabled;
}

QString CloudSyncManager::owncloudServer() const
{
    return d->m_owncloudProtocol + d->m_ownloudServer;
}

QString CloudSyncManager::owncloudUsername() const
{
    return d->m_owncloudUsername;
}

QString CloudSyncManager::owncloudPassword() const
{
    return d->m_owncloudPassword;
}

void CloudSyncManager::setSyncEnabled(bool enabled)
{
    if (d->m_syncEnabled != enabled) {
        d->m_syncEnabled = enabled;
        Q_EMIT syncEnabledChanged(d->m_syncEnabled);
    }
}

void CloudSyncManager::setOwncloudCredentials(const QString &server, const QString &user, const QString &password)
{
    QUrl const oldApiUrl = apiUrl();
    blockSignals(true);
    setOwncloudServer(server);
    setOwncloudUsername(user);
    setOwncloudPassword(password);
    blockSignals(false);
    if (oldApiUrl != apiUrl()) {
        Q_EMIT apiUrlChanged(apiUrl());
    }
}

void CloudSyncManager::setOwncloudServer(const QString &server)
{
    QString const oldProtocol = d->m_owncloudProtocol;
    QString const oldServer = d->m_ownloudServer;

    if (server.startsWith(QLatin1StringView("http://"))) {
        d->m_owncloudProtocol = QStringLiteral("http://");
        d->m_ownloudServer = server.mid(7);
    } else if (server.startsWith(QLatin1StringView("https://"))) {
        d->m_owncloudProtocol = QStringLiteral("https://");
        d->m_ownloudServer = server.mid(8);
    } else {
        d->m_owncloudProtocol = QStringLiteral("http://");
        d->m_ownloudServer = server;
    }

    if (oldServer != d->m_ownloudServer) {
        Q_EMIT owncloudServerChanged(owncloudServer());
        Q_EMIT apiUrlChanged(apiUrl());
    } else if (oldProtocol != d->m_owncloudProtocol) {
        Q_EMIT apiUrlChanged(apiUrl());
    }
}

void CloudSyncManager::setOwncloudUsername(const QString &username)
{
    if (d->m_owncloudUsername != username) {
        d->m_owncloudUsername = username;
        Q_EMIT owncloudUsernameChanged(d->m_owncloudUsername);
        Q_EMIT apiUrlChanged(apiUrl());
    }
}

void CloudSyncManager::setOwncloudPassword(const QString &password)
{
    if (d->m_owncloudPassword != password) {
        d->m_owncloudPassword = password;
        Q_EMIT owncloudPasswordChanged(d->m_owncloudPassword);
        Q_EMIT apiUrlChanged(apiUrl());
    }
}

void CloudSyncManager::setStatus(const QString &status, CloudSyncManager::Status status_type)
{
    if (d->m_status != status && d->m_status_type != status_type) {
        d->m_status = status;
        d->m_status_type = status_type;
        Q_EMIT statusChanged(d->m_status);
    }
}

CloudSyncManager::Status CloudSyncManager::status() const
{
    return d->m_status_type;
}

QString CloudSyncManager::statusDescription() const
{
    return d->m_status;
}

QString CloudSyncManager::apiPath() const
{
    return QStringLiteral("index.php/apps/marble/api/v1");
}

QUrl CloudSyncManager::apiUrl() const
{
    return QUrl(QStringLiteral("%1%2:%3@%4/%5")
                    .arg(d->m_owncloudProtocol)
                    .arg(d->m_owncloudUsername)
                    .arg(d->m_owncloudPassword)
                    .arg(d->m_ownloudServer)
                    .arg(apiPath()));
}

RouteSyncManager *CloudSyncManager::routeSyncManager()
{
    if (!d->m_routeSyncManager) {
        d->m_routeSyncManager = new RouteSyncManager(this);
    }

    return d->m_routeSyncManager;
}

BookmarkSyncManager *CloudSyncManager::bookmarkSyncManager()
{
    if (!d->m_bookmarkSyncManager) {
        d->m_bookmarkSyncManager = new BookmarkSyncManager(this);
    }

    return d->m_bookmarkSyncManager;
}

}

#include "moc_CloudSyncManager.cpp"
