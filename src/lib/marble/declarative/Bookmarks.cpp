// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include "Bookmarks.h"

#include "BookmarkManager.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTreeModel.h"
#include "GeoDataTypes.h"
#include "MarbleModel.h"
#include "MarblePlacemarkModel.h"
#include "MarbleQuickItem.h"
#include "Planet.h"
#include "kdescendantsproxymodel.h"

namespace Marble
{

Bookmarks::Bookmarks(QObject *parent)
    : QObject(parent)
{
    // nothing to do
}

MarbleQuickItem *Bookmarks::map()
{
    return m_marbleQuickItem;
}

void Bookmarks::setMap(MarbleQuickItem *item)
{
    m_marbleQuickItem = item;
    if (item) {
        connect(item->model()->bookmarkManager(), SIGNAL(bookmarksChanged()), this, SLOT(updateBookmarkDocument()));
    }
    updateBookmarkDocument();
    Q_EMIT modelChanged();
}

bool Bookmarks::isBookmark(qreal longitude, qreal latitude) const
{
    if (!m_marbleQuickItem || !m_marbleQuickItem->model()->bookmarkManager()) {
        return false;
    }

    Marble::BookmarkManager *manager = m_marbleQuickItem->model()->bookmarkManager();
    Marble::GeoDataDocument *bookmarks = manager->document();
    Marble::GeoDataCoordinates const compareTo(longitude, latitude, 0.0, Marble::GeoDataCoordinates::Degree);

    qreal planetRadius = m_marbleQuickItem->model()->planet()->radius();
    for (const Marble::GeoDataFolder *folder : bookmarks->folderList()) {
        for (const Marble::GeoDataPlacemark *const placemark : folder->placemarkList()) {
            if (placemark->coordinate().sphericalDistanceTo(compareTo) * planetRadius < 5) {
                return true;
            }
        }
    }

    return false;
}

Placemark *Bookmarks::placemark(int row)
{
    auto placemark = new Placemark;

    QModelIndex index = model()->index(row, 0);
    auto object = model()->data(index, MarblePlacemarkModel::ObjectPointerRole).value<GeoDataObject *>();
    if (auto geoDataPlacemark = geodata_cast<GeoDataPlacemark>(object)) {
        placemark->setGeoDataPlacemark(*geoDataPlacemark);
    }

    return placemark;
}

void Bookmarks::addBookmark(Placemark *placemark, const QString &folderName)
{
    if (!m_marbleQuickItem || !m_marbleQuickItem->model()->bookmarkManager()) {
        return;
    }

    Marble::BookmarkManager *manager = m_marbleQuickItem->model()->bookmarkManager();
    Marble::GeoDataDocument *bookmarks = manager->document();
    Marble::GeoDataContainer *target = nullptr;
    for (Marble::GeoDataFolder *const folder : bookmarks->folderList()) {
        if (folder->name() == folderName) {
            target = folder;
            break;
        }
    }

    if (!target) {
        manager->addNewBookmarkFolder(bookmarks, folderName);

        for (Marble::GeoDataFolder *const folder : bookmarks->folderList()) {
            if (folder->name() == folderName) {
                target = folder;
                break;
            }
        }

        Q_ASSERT(target);
    }

    Marble::GeoDataPlacemark bookmark = placemark->placemark();
    if (bookmark.name().isEmpty()) {
        bookmark.setName(placemark->address());
    }
    if (bookmark.name().isEmpty()) {
        bookmark.setName(bookmark.coordinate().toString(GeoDataCoordinates::Decimal).trimmed());
    }
    bookmark.clearOsmData();
    bookmark.setCoordinate(bookmark.coordinate()); // replace non-point geometries with their center
    manager->addBookmark(target, bookmark);
}

void Bookmarks::removeBookmark(qreal longitude, qreal latitude)
{
    if (!m_marbleQuickItem || !m_marbleQuickItem->model()->bookmarkManager()) {
        return;
    }

    Marble::BookmarkManager *manager = m_marbleQuickItem->model()->bookmarkManager();
    Marble::GeoDataDocument *bookmarks = manager->document();
    Marble::GeoDataCoordinates const compareTo(longitude, latitude, 0.0, Marble::GeoDataCoordinates::Degree);

    qreal planetRadius = m_marbleQuickItem->model()->planet()->radius();
    for (const Marble::GeoDataFolder *folder : bookmarks->folderList()) {
        for (Marble::GeoDataPlacemark *placemark : folder->placemarkList()) {
            if (placemark->coordinate().sphericalDistanceTo(compareTo) * planetRadius < 5) {
                manager->removeBookmark(placemark);
                return;
            }
        }
    }
}

void Bookmarks::updateBookmarkDocument()
{
    if (m_marbleQuickItem) {
        Marble::BookmarkManager *manager = m_marbleQuickItem->model()->bookmarkManager();
        m_treeModel.setRootDocument(manager->document());
    }
}

BookmarksModel *Bookmarks::model()
{
    if (!m_proxyModel && m_marbleQuickItem && m_marbleQuickItem->model()->bookmarkManager()) {
        auto flattener = new KDescendantsProxyModel(this);
        flattener->setSourceModel(&m_treeModel);

        m_proxyModel = new BookmarksModel(this);
        m_proxyModel->setFilterFixedString(QString::fromLatin1(Marble::GeoDataTypes::GeoDataPlacemarkType));
        m_proxyModel->setFilterKeyColumn(1);
        m_proxyModel->setSourceModel(flattener);
    }

    return m_proxyModel;
}

BookmarksModel::BookmarksModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    connect(this, SIGNAL(layoutChanged()), this, SIGNAL(countChanged()));
    connect(this, SIGNAL(modelReset()), this, SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SIGNAL(countChanged()));
}

int BookmarksModel::count() const
{
    return rowCount();
}

qreal BookmarksModel::longitude(int idx) const
{
    if (idx >= 0 && idx < rowCount()) {
        QVariant const value = data(index(idx, 0), Marble::MarblePlacemarkModel::CoordinateRole);
        auto const coordinates = value.value<Marble::GeoDataCoordinates>();
        return coordinates.longitude(Marble::GeoDataCoordinates::Degree);
    }
    return 0.0;
}

qreal BookmarksModel::latitude(int idx) const
{
    if (idx >= 0 && idx < rowCount()) {
        QVariant const value = data(index(idx, 0), Marble::MarblePlacemarkModel::CoordinateRole);
        auto const coordinates = value.value<Marble::GeoDataCoordinates>();
        return coordinates.latitude(Marble::GeoDataCoordinates::Degree);
    }
    return 0.0;
}

QString BookmarksModel::name(int idx) const
{
    if (idx >= 0 && idx < rowCount()) {
        return data(index(idx, 0)).toString();
    }
    return {};
}

}

#include "moc_Bookmarks.cpp"
