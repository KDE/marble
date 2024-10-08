// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DECLARATIVE_BOOKMARKS_H
#define MARBLE_DECLARATIVE_BOOKMARKS_H

#include "Placemark.h"
#include <GeoDataTreeModel.h>
#include <QObject>
#include <QSortFilterProxyModel>
#include <qqmlintegration.h>

namespace Marble
{

class MarbleQuickItem;

class BookmarksModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit BookmarksModel(QObject *parent = nullptr);

    int count() const;

public Q_SLOTS:
    qreal longitude(int index) const;

    qreal latitude(int index) const;

    QString name(int index) const;

Q_SIGNALS:
    void countChanged();
};

class Bookmarks : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(Marble::MarbleQuickItem *map READ map WRITE setMap NOTIFY mapChanged)
    Q_PROPERTY(BookmarksModel *model READ model NOTIFY modelChanged)

public:
    explicit Bookmarks(QObject *parent = nullptr);

    MarbleQuickItem *map();

    void setMap(MarbleQuickItem *widget);

    BookmarksModel *model();

    Q_INVOKABLE bool isBookmark(qreal longitude, qreal latitude) const;

    Q_INVOKABLE Placemark *placemark(int index);

public Q_SLOTS:
    void addBookmark(Placemark *placemark, const QString &folder);
    void removeBookmark(qreal longitude, qreal latitude);
    void updateBookmarkDocument();

Q_SIGNALS:
    void mapChanged();

    void modelChanged();

private:
    MarbleQuickItem *m_marbleQuickItem = nullptr;
    BookmarksModel *m_proxyModel = nullptr;
    GeoDataTreeModel m_treeModel;
};

}

#endif
