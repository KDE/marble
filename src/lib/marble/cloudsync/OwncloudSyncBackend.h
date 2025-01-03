// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef OWNCLOUDSYNCBACKEND_H
#define OWNCLOUDSYNCBACKEND_H

#include <QList>
#include <QNetworkReply>
#include <QObject>

class QUrl;
class QDir;

namespace Marble
{

class CloudSyncManager;
class RouteItem;

class OwncloudSyncBackend : public QObject
{
    Q_OBJECT

public:
    explicit OwncloudSyncBackend(CloudSyncManager *cloudSyncManager);
    ~OwncloudSyncBackend() override;

    /**
     * Generates an endpoint URL by appending endpoint name to API URL
     * @param endpoint Endpoint name which will be appended to API URL
     * @return QUrl which can be used for interactions with API
     */
    QUrl endpointUrl(const QString &endpoint) const;

    /**
     * Generates an endpoint URL by appending endpoint name and parameter to API URL
     * @param endpoint Endpoint name which will be appended to API URL
     * @param parameter Parameter which will be appended to API URL right after endpoint
     * @return QUrl which can be used for interactions with API
     */
    QUrl endpointUrl(const QString &endpoint, const QString &parameter) const;

    /**
     * Removes route with given timestamp from cache
     * @param cacheDir Local synchronization cache directory
     * @param timestamp Timestamp of the route which will be deleted
     */
    void removeFromCache(const QDir &cacheDir, const QString &timestamp);

    void uploadRoute(const QString &timestamp);
    void downloadRouteList();
    void downloadRoute(const QString &timestamp);
    void deleteRoute(const QString &timestamp);
    QPixmap createPreview(const QString &timestamp) const;
    QString routeName(const QString &timestamp) const;

public Q_SLOTS:
    void cancelUpload();

private Q_SLOTS:
    void checkAuthReply();
    void checkAuthError(QNetworkReply::NetworkError error);
    void prepareRouteList();
    void saveDownloadedRoute();
    void validateSettings();

Q_SIGNALS:
    void routeListDownloaded(const QList<RouteItem> &routeList);
    void routeDownloaded();
    void routeDeleted();
    void routeUploadProgress(qint64 sent, qint64 total);
    void routeDownloadProgress(qint64 received, qint64 total);
    void routeListDownloadProgress(qint64 received, qint64 total);
    void removedFromCache(const QString &timestamp);

private:
    class Private;
    Private *const d;
};

}

#endif // OWNCLOUDSYNCBACKEND_H
