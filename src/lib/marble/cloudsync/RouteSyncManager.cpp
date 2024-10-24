// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Utku Aydın <utkuaydin34@gmail.com>
//

#include "RouteSyncManager.h"

#include "CloudRouteModel.h"
#include "CloudRoutesDialog.h"
#include "CloudSyncManager.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "OwncloudSyncBackend.h"
#include "RouteItem.h"
#include "RoutingManager.h"

#include <QDir>
#include <QFile>
#include <QIcon>
#include <QPointer>
#include <QUrl>

namespace Marble
{

/**
 * Private class for RouteSyncManager.
 */
class Q_DECL_HIDDEN RouteSyncManager::Private
{
public:
    Private(CloudSyncManager *cloudSyncManager);
    ~Private();

    bool m_routeSyncEnabled;
    CloudSyncManager *const m_cloudSyncManager;
    RoutingManager *m_routingManager = nullptr;
    CloudRouteModel *const m_model;

    QDir m_cacheDir;
    OwncloudSyncBackend m_owncloudBackend;
    QList<RouteItem> m_routeList;
};

RouteSyncManager::Private::Private(CloudSyncManager *cloudSyncManager)
    : m_routeSyncEnabled(false)
    , m_cloudSyncManager(cloudSyncManager)
    , m_model(new CloudRouteModel())
    , m_owncloudBackend(cloudSyncManager)
{
    m_cacheDir = QDir(MarbleDirs::localPath() + QLatin1StringView("/cloudsync/cache/routes/"));
}

RouteSyncManager::Private::~Private()
{
    delete m_model;
}

RouteSyncManager::RouteSyncManager(CloudSyncManager *cloudSyncManager)
    : d(new Private(cloudSyncManager))
{
    connect(&d->m_owncloudBackend, &OwncloudSyncBackend::routeUploadProgress, this, &RouteSyncManager::updateUploadProgressbar);
    connect(&d->m_owncloudBackend, &OwncloudSyncBackend::routeListDownloaded, this, &RouteSyncManager::setRouteModelItems);
    connect(&d->m_owncloudBackend, &OwncloudSyncBackend::routeListDownloadProgress, this, &RouteSyncManager::routeListDownloadProgress);
    connect(&d->m_owncloudBackend, &OwncloudSyncBackend::routeDownloadProgress, d->m_model, &CloudRouteModel::updateProgress);
    connect(&d->m_owncloudBackend, &OwncloudSyncBackend::routeDownloaded, this, &RouteSyncManager::prepareRouteList);
    connect(&d->m_owncloudBackend, &OwncloudSyncBackend::routeDeleted, this, &RouteSyncManager::prepareRouteList);
    connect(&d->m_owncloudBackend, &OwncloudSyncBackend::removedFromCache, this, &RouteSyncManager::prepareRouteList);
}

RouteSyncManager::~RouteSyncManager()
{
    delete d;
}

void RouteSyncManager::setRoutingManager(RoutingManager *routingManager)
{
    d->m_routingManager = routingManager;
}

bool RouteSyncManager::isRouteSyncEnabled() const
{
    return d->m_routeSyncEnabled && d->m_cloudSyncManager && d->m_cloudSyncManager->isSyncEnabled();
}

void RouteSyncManager::setRouteSyncEnabled(bool enabled)
{
    if (d->m_routeSyncEnabled != enabled) {
        d->m_routeSyncEnabled = enabled;
        Q_EMIT routeSyncEnabledChanged(d->m_routeSyncEnabled);
    }
}

CloudRouteModel *RouteSyncManager::model()
{
    return d->m_model;
}

QString RouteSyncManager::generateTimestamp() const
{
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    return QString::number(timestamp);
}

QString RouteSyncManager::saveDisplayedToCache() const
{
    if (!d->m_routingManager) {
        qWarning() << "RoutingManager instance not set in RouteSyncManager. Cannot save current route.";
        return {};
    }

    d->m_cacheDir.mkpath(d->m_cacheDir.absolutePath());

    const QString timestamp = generateTimestamp();
    const QString filename = d->m_cacheDir.absolutePath() + QLatin1Char('/') + timestamp + QLatin1StringView(".kml");
    d->m_routingManager->saveRoute(filename);
    return timestamp;
}

void RouteSyncManager::uploadRoute()
{
    if (!d->m_cloudSyncManager->workOffline()) {
        d->m_owncloudBackend.uploadRoute(saveDisplayedToCache());
    }
}

QList<RouteItem> RouteSyncManager::cachedRouteList() const
{
    QList<RouteItem> routeList;
    QStringList cachedRoutes = d->m_cacheDir.entryList(QStringList() << QStringLiteral("*.kml"), QDir::Files);
    for (const QString &routeFilename : std::as_const(cachedRoutes)) {
        QFile file(d->m_cacheDir.absolutePath() + QLatin1Char('/') + routeFilename);
        file.open(QFile::ReadOnly);

        GeoDataParser parser(GeoData_KML);
        if (!parser.read(&file)) {
            mDebug() << QLatin1StringView("Could not read ") + routeFilename;
        }

        file.close();

        QString routeName;
        GeoDocument *geoDoc = parser.releaseDocument();
        auto container = dynamic_cast<GeoDataDocument *>(geoDoc);
        if (container && !container->isEmpty()) {
            GeoDataFolder *folder = container->folderList().at(0);
            for (GeoDataPlacemark *placemark : folder->placemarkList()) {
                routeName += placemark->name() + QLatin1StringView(" - ");
            }
        }

        routeName = routeName.left(routeName.length() - 3);
        QString timestamp = routeFilename.left(routeFilename.length() - 4);
        QString distance(QLatin1Char('0'));
        QString duration(QLatin1Char('0'));

        QString previewPath = QStringLiteral("%0/preview/%1.jpg").arg(d->m_cacheDir.absolutePath(), timestamp);
        QIcon preview;

        if (QFile(previewPath).exists()) {
            preview = QIcon(previewPath);
        }

        // Would that work on Windows?
        QUrl previewUrl(QStringLiteral("file://%0").arg(previewPath));

        RouteItem item;
        item.setIdentifier(timestamp);
        item.setName(routeName);
        item.setDistance(distance);
        item.setDistance(duration);
        item.setPreview(preview);
        item.setPreviewUrl(previewUrl);
        item.setOnCloud(false);
        routeList.append(item);
    }

    return routeList;
}

void RouteSyncManager::uploadRoute(const QString &timestamp)
{
    if (!d->m_cloudSyncManager->workOffline()) {
        d->m_owncloudBackend.uploadRoute(timestamp);
    }
}

void RouteSyncManager::prepareRouteList()
{
    d->m_routeList.clear();

    QList<RouteItem> cachedRoutes = cachedRouteList();
    for (const RouteItem &item : std::as_const(cachedRoutes)) {
        d->m_routeList.append(item);
    }

    if (!d->m_cloudSyncManager->workOffline()) {
        d->m_owncloudBackend.downloadRouteList();
    } else {
        // If not offline, setRouteModelItems() does this after
        // appending downloaded items to the list.
        d->m_model->setItems(d->m_routeList);
    }
}

void RouteSyncManager::downloadRoute(const QString &timestamp)
{
    d->m_owncloudBackend.downloadRoute(timestamp);
}

void RouteSyncManager::openRoute(const QString &timestamp)
{
    if (!d->m_routingManager) {
        qWarning() << "RoutingManager instance not set in RouteSyncManager. Cannot open route " << timestamp;
        return;
    }

    d->m_routingManager->loadRoute(QStringLiteral("%0/%1.kml").arg(d->m_cacheDir.absolutePath()).arg(timestamp));
}

void RouteSyncManager::deleteRoute(const QString &timestamp)
{
    d->m_owncloudBackend.deleteRoute(timestamp);
}

void RouteSyncManager::removeRouteFromCache(const QString &timestamp)
{
    d->m_owncloudBackend.removeFromCache(d->m_cacheDir, timestamp);
}

void RouteSyncManager::updateUploadProgressbar(qint64 sent, qint64 total)
{
    Q_EMIT routeUploadProgress(sent, total);
    if (sent == total) {
        prepareRouteList();
    }
}

void RouteSyncManager::setRouteModelItems(const QList<RouteItem> &routeList)
{
    if (d->m_routeList.count() > 0) {
        QStringList cloudRoutes;
        for (const RouteItem &item : routeList) {
            cloudRoutes.append(item.identifier());
        }

        for (int position = 0; position < d->m_routeList.count(); position++) {
            if (cloudRoutes.contains(d->m_routeList.at(position).identifier())) {
                d->m_routeList[position].setOnCloud(true);
            }
        }

        QStringList cachedRoutes;
        for (const RouteItem &item : std::as_const(d->m_routeList)) {
            cachedRoutes.append(item.identifier());
        }

        for (const RouteItem &item : routeList) {
            if (!cachedRoutes.contains(item.identifier())) {
                d->m_routeList.append(item);
            }
        }
    } else {
        for (const RouteItem &item : routeList) {
            d->m_routeList.append(item);
        }
    }

    d->m_model->setItems(d->m_routeList);
}

}

#include "moc_RouteSyncManager.cpp"
