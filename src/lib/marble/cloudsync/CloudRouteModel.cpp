// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Utku Aydın <utkuaydin34@gmail.com>
//

#include "CloudRouteModel.h"

#include "RouteItem.h"

#include "MarbleDirs.h"

#include <QIcon>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSet>
#include <QUrl>

namespace Marble
{

class Q_DECL_HIDDEN CloudRouteModel::Private
{
public:
    Private();

    QList<RouteItem> m_items;
    QString m_cacheDir;
    QPersistentModelIndex m_downloading;
    qint64 m_totalSize;
    qint64 m_downloadedSize;

    QNetworkAccessManager m_network;
    QMap<QNetworkReply *, int> m_previewQueue;
    QSet<QString> m_requestedPreviews;
};

CloudRouteModel::Private::Private()
    : m_totalSize(-1)
    , m_downloadedSize(0)
{
    m_cacheDir = MarbleDirs::localPath() + QLatin1StringView("/cloudsync/cache/routes/");
}

CloudRouteModel::CloudRouteModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private())
{
    connect(&(d->m_network), &QNetworkAccessManager::finished, this, &CloudRouteModel::setPreview);
}

CloudRouteModel::~CloudRouteModel() = default;

QVariant CloudRouteModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));
    switch (role) {
    case Qt::DecorationRole:
        return preview(index);
    case Timestamp:
        return d->m_items.at(index.row()).identifier();
    case Name:
        return d->m_items.at(index.row()).name();
    case PreviewUrl:
        return d->m_items.at(index.row()).previewUrl();
    case Distance:
        return d->m_items.at(index.row()).distance();
    case Duration:
        return d->m_items.at(index.row()).duration();
    case IsCached:
        return isCached(index);
    case IsDownloading:
        return isDownloading(index);
    case IsOnCloud:
        return d->m_items.at(index.row()).onCloud();
    default:
        return {};
    }
}

int CloudRouteModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : d->m_items.count();
}

QHash<int, QByteArray> CloudRouteModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[Name] = "name";
    roles[Timestamp] = "identifier";
    roles[PreviewUrl] = "previewUrl";
    roles[Distance] = "distance";
    roles[Duration] = "duration";
    roles[IsCached] = "isCached";
    roles[IsDownloading] = "isDownloading";
    roles[IsOnCloud] = "isOnCloud";
    return roles;
}

void CloudRouteModel::setItems(const QList<RouteItem> &items)
{
    beginResetModel();
    d->m_items = items;
    d->m_previewQueue.clear();
    d->m_requestedPreviews.clear();
    endResetModel();
}

bool CloudRouteModel::isCached(const QModelIndex &index) const
{
    return QFileInfo::exists(d->m_cacheDir + index.data(Timestamp).toString() + QLatin1StringView(".kml"));
}

QPersistentModelIndex CloudRouteModel::downloadingItem() const
{
    return d->m_downloading;
}

void CloudRouteModel::setDownloadingItem(const QPersistentModelIndex &index)
{
    d->m_downloading = index;
}

bool CloudRouteModel::isDownloading(const QModelIndex &index) const
{
    return d->m_downloading == index;
}

qint64 CloudRouteModel::totalSize() const
{
    return d->m_totalSize;
}

qint64 CloudRouteModel::downloadedSize() const
{
    return d->m_downloadedSize;
}

QIcon CloudRouteModel::preview(const QModelIndex &index) const
{
    QString timestamp = d->m_items.at(index.row()).identifier();
    if (d->m_items.at(index.row()).preview().isNull() && !d->m_requestedPreviews.contains(timestamp)) {
        QUrl url(d->m_items.at(index.row()).previewUrl());
        QNetworkRequest request(url);
        QNetworkReply *reply = d->m_network.get(request);
        d->m_previewQueue.insert(reply, index.row());
        d->m_requestedPreviews.insert(timestamp);
    }

    return d->m_items.at(index.row()).preview();
}

void CloudRouteModel::setPreview(QNetworkReply *reply)
{
    int position = d->m_previewQueue.take(reply);

    if (position >= d->m_items.count()) {
        return;
    }

    RouteItem &route = d->m_items[position];
    QIcon icon(QPixmap::fromImage(QImage::fromData(reply->readAll())));
    route.setPreview(icon);
    d->m_requestedPreviews.remove(route.identifier());
}

void CloudRouteModel::updateProgress(qint64 currentSize, qint64 totalSize)
{
    d->m_totalSize = totalSize;
    d->m_downloadedSize = currentSize;
    dataChanged(d->m_downloading, d->m_downloading);
    if (currentSize == totalSize) {
        d->m_downloading = QPersistentModelIndex();
        d->m_totalSize = -1;
        d->m_downloadedSize = 0;
    }
}

}

#include "moc_CloudRouteModel.cpp"
