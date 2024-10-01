// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef CLOUDROUTEMODEL_H
#define CLOUDROUTEMODEL_H

#include "marble_export.h"

#include <QAbstractListModel>
#include <QModelIndex>

class QNetworkReply;

namespace Marble
{

class RouteItem;

class MARBLE_EXPORT CloudRouteModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RouteRoles {
        Timestamp = Qt::UserRole + 1,
        Name,
        PreviewUrl,
        Distance,
        Duration,
        IsCached,
        IsDownloading,
        IsOnCloud,
    };

    explicit CloudRouteModel(QObject *parent = nullptr);
    ~CloudRouteModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /** Overload of QAbstractListModel */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * Sets the list of routes that will show up in CloudRoutesDialog.
     * @param items List of routes.
     */
    void setItems(const QList<RouteItem> &items);

    /**
     * Checks if specified route exists in the local cache.
     * @param index Index of the route.
     * @return true, if exists.
     */
    bool isCached(const QModelIndex &index) const;

    /**
     * Getter for the item currently being downloaded.
     * @return Model for the item currently being downloaded
     */
    QPersistentModelIndex downloadingItem() const;

    /**
     * Marks the route at given index as being downloaded.
     * @param index Index of the route.
     */
    void setDownloadingItem(const QPersistentModelIndex &index);

    /**
     * Checks if route is being downloaded.
     * @param index Index of the route.
     * @return true, if downloading.
     */
    bool isDownloading(const QModelIndex &index) const;

    /**
     * Total size of the item currently being downloaded.
     * @return Total size of the item, -1 if no route is being downloaded
     */
    qint64 totalSize() const;

    /**
     * Returns how much of the route are downloaded as bytes
     * @return Downloaded bytes
     */
    qint64 downloadedSize() const;

    /**
     * Checks whether a preview for the route available and
     * returns or downloads the preview
     * @param index Index of the item whose preview is requested
     * @return Route's preview as QIcon
     */
    QIcon preview(const QModelIndex &index) const;

public Q_SLOTS:
    void updateProgress(qint64 currentSize, qint64 totalSize);
    void setPreview(QNetworkReply *reply);

private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif // CLOUDROUTEMODEL_H
