// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "AbstractDataPluginModel.h"

// Qt
#include <QAbstractListModel>
#include <QMetaProperty>
#include <QPointF>
#include <QRectF>
#include <QTimer>
#include <QVariant>
#include <QtAlgorithms>

// Marble
#include "AbstractDataPluginItem.h"
#include "CacheStoragePolicy.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "HttpDownloadManager.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "ViewportParams.h"

#include <cmath>

namespace Marble
{

const QString descriptionPrefix(QStringLiteral("description_"));

// Time between two tried description file downloads (we decided not to download anything) in ms
const int timeBetweenTriedDownloads = 500;
// Time between two real description file downloads in ms
const int timeBetweenDownloads = 1500;

// The factor describing how much the box has to be changed to download a new description file.
// A higher factor means more downloads.
const qreal boxComparisonFactor = 16.0;

// Separator to separate the id of the item from the file type
const QChar fileIdSeparator = QLatin1Char('_');

class FavoritesModel;

class AbstractDataPluginModelPrivate
{
public:
    AbstractDataPluginModelPrivate(const QString &name, const MarbleModel *marbleModel, AbstractDataPluginModel *parent);

    ~AbstractDataPluginModelPrivate();

    static QString generateFilename(const QString &id, const QString &type);
    QString generateFilepath(const QString &id, const QString &type) const;

    void updateFavoriteItems();

    AbstractDataPluginModel *m_parent = nullptr;
    const QString m_name;
    const MarbleModel *const m_marbleModel;
    GeoDataLatLonAltBox m_lastBox;
    GeoDataLatLonAltBox m_downloadedBox;
    qint32 m_lastNumber;
    qint32 m_downloadedNumber;
    QString m_currentPlanetId;
    QList<AbstractDataPluginItem *> m_itemSet;
    QHash<QString, AbstractDataPluginItem *> m_downloadingItems;
    QList<AbstractDataPluginItem *> m_displayedItems;
    QTimer m_downloadTimer;
    quint32 m_descriptionFileNumber;
    QHash<QString, QVariant> m_itemSettings;
    QStringList m_favoriteItems;
    bool m_favoriteItemsOnly;

    CacheStoragePolicy m_storagePolicy;
    HttpDownloadManager m_downloadManager;
    FavoritesModel *m_favoritesModel;
    QMetaObject m_metaObject;
    bool m_hasMetaObject;
    bool m_needsSorting;
};

class FavoritesModel : public QAbstractListModel
{
public:
    AbstractDataPluginModelPrivate *const d;

    explicit FavoritesModel(AbstractDataPluginModelPrivate *d, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void reset();

    QHash<int, QByteArray> roleNames() const override;

private:
    QHash<int, QByteArray> m_roleNames;
};

AbstractDataPluginModelPrivate::AbstractDataPluginModelPrivate(const QString &name, const MarbleModel *marbleModel, AbstractDataPluginModel *parent)
    : m_parent(parent)
    , m_name(name)
    , m_marbleModel(marbleModel)
    , m_lastBox()
    , m_downloadedBox()
    , m_lastNumber(0)
    , m_downloadedNumber(0)
    , m_currentPlanetId(marbleModel->planetId())
    , m_downloadTimer(m_parent)
    , m_descriptionFileNumber(0)
    , m_itemSettings()
    , m_favoriteItemsOnly(false)
    , m_storagePolicy(MarbleDirs::localPath() + QLatin1StringView("/cache/") + m_name + QLatin1Char('/'))
    , m_downloadManager(&m_storagePolicy)
    , m_favoritesModel(nullptr)
    , m_hasMetaObject(false)
    , m_needsSorting(false)
{
}

AbstractDataPluginModelPrivate::~AbstractDataPluginModelPrivate()
{
    QList<AbstractDataPluginItem *>::iterator lIt = m_itemSet.begin();
    QList<AbstractDataPluginItem *>::iterator const lItEnd = m_itemSet.end();
    for (; lIt != lItEnd; ++lIt) {
        (*lIt)->deleteLater();
    }

    QHash<QString, AbstractDataPluginItem *>::iterator hIt = m_downloadingItems.begin();
    QHash<QString, AbstractDataPluginItem *>::iterator const hItEnd = m_downloadingItems.end();
    for (; hIt != hItEnd; ++hIt) {
        (*hIt)->deleteLater();
    }

    m_storagePolicy.clearCache();
}

void AbstractDataPluginModelPrivate::updateFavoriteItems()
{
    if (m_favoriteItemsOnly) {
        for (const QString &id : std::as_const(m_favoriteItems)) {
            if (!m_parent->findItem(id)) {
                m_parent->getItem(id);
            }
        }
    }
}

void AbstractDataPluginModel::themeChanged()
{
    if (d->m_currentPlanetId != d->m_marbleModel->planetId()) {
        clear();
        d->m_currentPlanetId = d->m_marbleModel->planetId();
    }
}

static bool lessThanByPointer(const AbstractDataPluginItem *item1, const AbstractDataPluginItem *item2)
{
    if (item1 && item2) {
        // Compare by sticky and favorite status (sticky first, then favorites), last by operator<
        bool const sticky1 = item1->isSticky();
        bool const favorite1 = item1->isFavorite();
        if (sticky1 != item2->isSticky()) {
            return sticky1;
        } else if (favorite1 != item2->isFavorite()) {
            return favorite1;
        } else {
            return item1->operator<(item2);
        }
    } else {
        return false;
    }
}

FavoritesModel::FavoritesModel(AbstractDataPluginModelPrivate *_d, QObject *parent)
    : QAbstractListModel(parent)
    , d(_d)
{
    QHash<int, QByteArray> roles;
    int const size = d->m_hasMetaObject ? d->m_metaObject.propertyCount() : 0;
    for (int i = 0; i < size; ++i) {
        QMetaProperty property = d->m_metaObject.property(i);
        roles[Qt::UserRole + i] = property.name();
    }
    roles[Qt::DisplayRole] = "display";
    roles[Qt::DecorationRole] = "decoration";
    m_roleNames = roles;
}

int FavoritesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    int count = 0;
    for (AbstractDataPluginItem *item : std::as_const(d->m_itemSet)) {
        if (item->initialized() && item->isFavorite()) {
            ++count;
        }
    }

    return count;
}

QVariant FavoritesModel::data(const QModelIndex &index, int role) const
{
    int const row = index.row();
    if (row >= 0 && row < rowCount()) {
        int count = 0;
        for (AbstractDataPluginItem *item : std::as_const(d->m_itemSet)) {
            if (item->initialized() && item->isFavorite()) {
                if (count == row) {
                    QString const roleName = QString::fromUtf8(roleNames().value(role));
                    return item->property(roleName.toLatin1().constData());
                }
                ++count;
            }
        }
    }

    return {};
}

void FavoritesModel::reset()
{
    beginResetModel();
    endResetModel();
}

QHash<int, QByteArray> FavoritesModel::roleNames() const
{
    return m_roleNames;
}

AbstractDataPluginModel::AbstractDataPluginModel(const QString &name, const MarbleModel *marbleModel, QObject *parent)
    : QObject(parent)
    , d(new AbstractDataPluginModelPrivate(name, marbleModel, this))
{
    Q_ASSERT(marbleModel != nullptr);

    // Initializing file and download System
    connect(&d->m_downloadManager, SIGNAL(downloadComplete(QString, QString)), this, SLOT(processFinishedJob(QString, QString)));

    connect(marbleModel, SIGNAL(themeChanged(QString)), this, SLOT(themeChanged()));

    // We want to download a new description file every timeBetweenDownloads ms
    connect(&d->m_downloadTimer, SIGNAL(timeout()), this, SLOT(handleChangedViewport()), Qt::QueuedConnection);
    d->m_downloadTimer.start(timeBetweenDownloads);
}

AbstractDataPluginModel::~AbstractDataPluginModel()
{
    delete d;
}

const MarbleModel *AbstractDataPluginModel::marbleModel() const
{
    return d->m_marbleModel;
}

QList<AbstractDataPluginItem *> AbstractDataPluginModel::items(const ViewportParams *viewport, qint32 number)
{
    GeoDataLatLonAltBox currentBox = viewport->viewLatLonAltBox();
    QList<AbstractDataPluginItem *> list;

    Q_ASSERT(!d->m_displayedItems.contains(nullptr) && "Null item in m_displayedItems. Please report a bug to marble-devel@kde.org");
    Q_ASSERT(!d->m_itemSet.contains(nullptr) && "Null item in m_itemSet. Please report a bug to marble-devel@kde.org");

    QList<AbstractDataPluginItem *> candidates = d->m_displayedItems + d->m_itemSet;

    if (d->m_needsSorting) {
        // Both the candidates list and the list of all items need to be sorted
        std::sort(candidates.begin(), candidates.end(), lessThanByPointer);
        std::sort(d->m_itemSet.begin(), d->m_itemSet.end(), lessThanByPointer);
        d->m_needsSorting = false;
    }

    QList<AbstractDataPluginItem *>::const_iterator i = candidates.constBegin();
    QList<AbstractDataPluginItem *>::const_iterator end = candidates.constEnd();

    // Items that are already shown have the highest priority
    for (; i != end && list.size() < number; ++i) {
        // Only show items that are initialized
        if (!(*i)->initialized()) {
            continue;
        }

        // Hide non-favorite items if necessary
        if (d->m_favoriteItemsOnly && !(*i)->isFavorite()) {
            continue;
        }

        (*i)->setProjection(viewport);
        if ((*i)->positions().isEmpty()) {
            continue;
        }

        if (list.contains(*i)) {
            continue;
        }

        // If the item was added initially at a nearer position, they don't have priority,
        // because we zoomed out since then.
        bool const alreadyDisplayed = d->m_displayedItems.contains(*i);
        if (!alreadyDisplayed || (*i)->addedAngularResolution() >= viewport->angularResolution() || (*i)->isSticky()) {
            bool collides = false;
            int const length = list.length();
            for (int j = 0; !collides && j < length; ++j) {
                for (const QRectF &rect : list[j]->boundingRects()) {
                    for (const QRectF &itemRect : (*i)->boundingRects()) {
                        if (rect.intersects(itemRect))
                            collides = true;
                    }
                }
            }

            if (!collides) {
                list.append(*i);
                (*i)->setSettings(d->m_itemSettings);

                // We want to save the angular resolution of the first time the item got added.
                if (!alreadyDisplayed) {
                    (*i)->setAddedAngularResolution(viewport->angularResolution());
                }
            }
        }
        // TODO: Do we have to cleanup at some point? The list of all items keeps growing
    }

    d->m_lastBox = currentBox;
    d->m_lastNumber = number;
    d->m_displayedItems = list;
    return list;
}

QList<AbstractDataPluginItem *> AbstractDataPluginModel::whichItemAt(const QPoint &curpos)
{
    QList<AbstractDataPluginItem *> itemsAt;

    const QPointF curposF(curpos);
    for (AbstractDataPluginItem *item : std::as_const(d->m_displayedItems)) {
        if (item && item->contains(curposF)) {
            itemsAt.append(item);
        }
    }

    return itemsAt;
}

void AbstractDataPluginModel::parseFile(const QByteArray &file)
{
    Q_UNUSED(file);
}

void AbstractDataPluginModel::downloadItem(const QUrl &url, const QString &type, AbstractDataPluginItem *item)
{
    if (!item) {
        return;
    }

    QString id = d->generateFilename(item->id(), type);

    d->m_downloadManager.addJob(url, id, id, DownloadBrowse);
    d->m_downloadingItems.insert(id, item);
}

void AbstractDataPluginModel::downloadDescriptionFile(const QUrl &url)
{
    if (!url.isEmpty()) {
        QString name(descriptionPrefix);
        name += QString::number(d->m_descriptionFileNumber);

        d->m_downloadManager.addJob(url, name, name, DownloadBrowse);
        d->m_descriptionFileNumber++;
    }
}

void AbstractDataPluginModel::addItemToList(AbstractDataPluginItem *item)
{
    addItemsToList(QList<AbstractDataPluginItem *>() << item);
}

void AbstractDataPluginModel::addItemsToList(const QList<AbstractDataPluginItem *> &items)
{
    bool needsUpdate = false;
    bool favoriteChanged = false;
    for (AbstractDataPluginItem *item : items) {
        if (!item) {
            continue;
        }

        // If the item is already in our list, don't add it.
        if (d->m_itemSet.contains(item)) {
            continue;
        }

        if (itemExists(item->id())) {
            item->deleteLater();
            continue;
        }

        mDebug() << "New item " << item->id();

        // This find the right position in the sorted to insert the new item
        QList<AbstractDataPluginItem *>::iterator i = std::lower_bound(d->m_itemSet.begin(), d->m_itemSet.end(), item, lessThanByPointer);
        // Insert the item on the right position in the list
        d->m_itemSet.insert(i, item);

        connect(item, &AbstractDataPluginItem::stickyChanged, this, &AbstractDataPluginModel::scheduleItemSort);
        connect(item, &QObject::destroyed, this, &AbstractDataPluginModel::removeItem);
        connect(item, &AbstractDataPluginItem::updated, this, &AbstractDataPluginModel::itemsUpdated);
        connect(item, &AbstractDataPluginItem::favoriteChanged, this, &AbstractDataPluginModel::favoriteItemChanged);

        if (!needsUpdate && item->initialized()) {
            needsUpdate = true;
        }

        if (!favoriteChanged && item->initialized() && item->isFavorite()) {
            favoriteChanged = true;
        }
    }

    if (favoriteChanged && d->m_favoritesModel) {
        d->m_favoritesModel->reset();
    }

    if (needsUpdate) {
        Q_EMIT itemsUpdated();
    }
}

void AbstractDataPluginModel::getItem(const QString &)
{
    qWarning() << "Retrieving items by identifier is not implemented by this plugin";
}

void AbstractDataPluginModel::setFavoriteItems(const QStringList &list)
{
    if (d->m_favoriteItems != list) {
        d->m_favoriteItems = list;
        d->updateFavoriteItems();
        if (d->m_favoritesModel) {
            d->m_favoritesModel->reset();
        }
        Q_EMIT favoriteItemsChanged(d->m_favoriteItems);
    }
}

QStringList AbstractDataPluginModel::favoriteItems() const
{
    return d->m_favoriteItems;
}

void AbstractDataPluginModel::setFavoriteItemsOnly(bool favoriteOnly)
{
    if (isFavoriteItemsOnly() != favoriteOnly) {
        d->m_favoriteItemsOnly = favoriteOnly;
        d->updateFavoriteItems();
        Q_EMIT favoriteItemsOnlyChanged();
    }
}

bool AbstractDataPluginModel::isFavoriteItemsOnly() const
{
    return d->m_favoriteItemsOnly;
}

QObject *AbstractDataPluginModel::favoritesModel()
{
    if (!d->m_favoritesModel) {
        d->m_favoritesModel = new FavoritesModel(d, this);
        d->updateFavoriteItems();
    }

    return d->m_favoritesModel;
}

void AbstractDataPluginModel::favoriteItemChanged(const QString &id, bool isFavorite)
{
    QStringList favorites = d->m_favoriteItems;

    if (isFavorite) {
        if (!favorites.contains(id))
            favorites.append(id);
    } else {
        favorites.removeOne(id);
    }

    setFavoriteItems(favorites);
    scheduleItemSort();
}

void AbstractDataPluginModel::scheduleItemSort()
{
    d->m_needsSorting = true;
}

QString AbstractDataPluginModelPrivate::generateFilename(const QString &id, const QString &type)
{
    QString name;
    name += id;
    name += fileIdSeparator;
    name += type;

    return name;
}

QString AbstractDataPluginModelPrivate::generateFilepath(const QString &id, const QString &type) const
{
    return MarbleDirs::localPath() + QLatin1StringView("/cache/") + m_name + QLatin1Char('/') + generateFilename(id, type);
}

AbstractDataPluginItem *AbstractDataPluginModel::findItem(const QString &id) const
{
    for (AbstractDataPluginItem *item : std::as_const(d->m_itemSet)) {
        if (item->id() == id) {
            return item;
        }
    }

    return nullptr;
}

bool AbstractDataPluginModel::itemExists(const QString &id) const
{
    return findItem(id);
}

void AbstractDataPluginModel::setItemSettings(const QHash<QString, QVariant> &itemSettings)
{
    d->m_itemSettings = itemSettings;
}

void AbstractDataPluginModel::handleChangedViewport()
{
    if (d->m_favoriteItemsOnly) {
        return;
    }

    // All this is to prevent to often downloads
    if (d->m_lastNumber != 0
        // We don't need to download if nothing changed
        && (!(d->m_downloadedBox == d->m_lastBox) || d->m_downloadedNumber != d->m_lastNumber)
        // We try to filter little changes of the bounding box
        && (fabs(d->m_downloadedBox.east() - d->m_lastBox.east()) * boxComparisonFactor > d->m_lastBox.width()
            || fabs(d->m_downloadedBox.south() - d->m_lastBox.south()) * boxComparisonFactor > d->m_lastBox.height()
            || fabs(d->m_downloadedBox.north() - d->m_lastBox.north()) * boxComparisonFactor > d->m_lastBox.height()
            || fabs(d->m_downloadedBox.west() - d->m_lastBox.west()) * boxComparisonFactor > d->m_lastBox.width())) {
        // We will wait a little bit longer to start the
        // next download as we will really download something now.
        d->m_downloadTimer.setInterval(timeBetweenDownloads);

        // Save the download parameter
        d->m_downloadedBox = d->m_lastBox;
        d->m_downloadedNumber = d->m_lastNumber;

        // Get items
        getAdditionalItems(d->m_lastBox, d->m_lastNumber);
    } else {
        // Don't wait to long to start the next download as we decided not to download anything.
        // This will enhance response.
        d->m_downloadTimer.setInterval(timeBetweenTriedDownloads);
    }
}

void AbstractDataPluginModel::processFinishedJob(const QString &relativeUrlString, const QString &id)
{
    Q_UNUSED(relativeUrlString);

    if (id.startsWith(descriptionPrefix)) {
        parseFile(d->m_storagePolicy.data(id));
    } else {
        // The downloaded file contains item data.

        // Splitting the id in itemId and fileType
        QStringList fileInformation = id.split(fileIdSeparator);

        if (fileInformation.size() < 2) {
            mDebug() << "Strange file information " << id;
            return;
        }
        QString itemId = fileInformation.at(0);
        fileInformation.removeAt(0);
        QString fileType = fileInformation.join(QString(fileIdSeparator));

        // Searching for the right item in m_downloadingItems
        QHash<QString, AbstractDataPluginItem *>::iterator i = d->m_downloadingItems.find(id);
        if (i != d->m_downloadingItems.end()) {
            if (itemId != (*i)->id()) {
                return;
            }

            (*i)->addDownloadedFile(d->generateFilepath(itemId, fileType), fileType);

            d->m_downloadingItems.erase(i);
        }
    }
}

void AbstractDataPluginModel::removeItem(QObject *item)
{
    auto pluginItem = qobject_cast<AbstractDataPluginItem *>(item);
    d->m_itemSet.removeAll(pluginItem);
    QHash<QString, AbstractDataPluginItem *>::iterator i;
    for (i = d->m_downloadingItems.begin(); i != d->m_downloadingItems.end(); ++i) {
        if (*i == pluginItem) {
            i = d->m_downloadingItems.erase(i);
        }
    }
}

void AbstractDataPluginModel::clear()
{
    d->m_displayedItems.clear();
    QList<AbstractDataPluginItem *>::iterator iter = d->m_itemSet.begin();
    QList<AbstractDataPluginItem *>::iterator const end = d->m_itemSet.end();
    for (; iter != end; ++iter) {
        (*iter)->deleteLater();
    }
    d->m_itemSet.clear();
    d->m_lastBox = GeoDataLatLonAltBox();
    d->m_downloadedBox = GeoDataLatLonAltBox();
    d->m_downloadedNumber = 0;
    Q_EMIT itemsUpdated();
}

void AbstractDataPluginModel::registerItemProperties(const QMetaObject &item)
{
    d->m_metaObject = item;
    d->m_hasMetaObject = true;
}

} // namespace Marble

#include "moc_AbstractDataPluginModel.cpp"
