// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Claudiu Covaci <claudiu.covaci@gmail.com>
// SPDX-FileCopyrightText: 2009 Torsten Rahn <tackat@kde.org>
//

#include "MapThemeSortFilterProxyModel.h"

#include <QDateTime>
#include <QModelIndex>
#include <QSettings>
#include <QString>

/* TRANSLATOR Marble::MapThemeSortFilterProxyModel */

namespace Marble
{

MapThemeSortFilterProxyModel::MapThemeSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    // nothing to do
}

bool MapThemeSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (isFavorite(left)) {
        if (!isFavorite(right)) {
            return true;
        }
    } else {
        if (isFavorite(right)) {
            return false;
        }
    }
    return sourceModel()->data(left).toString() < sourceModel()->data(right).toString();
}

bool MapThemeSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    return (sourceModel()->data(index, Qt::UserRole + 1).toString().contains(filterRegularExpression()));
}

bool MapThemeSortFilterProxyModel::isFavorite(const QModelIndex &index)
{
    const QAbstractItemModel *model = index.model();
    QModelIndex columnIndex = model->index(index.row(), 0, QModelIndex());
    QString const key = QLatin1StringView("Favorites/") + model->data(columnIndex).toString();
    return QSettings().contains(key);
}

QDateTime MapThemeSortFilterProxyModel::favoriteDateTime(const QModelIndex &index)
{
    const QAbstractItemModel *model = index.model();
    QModelIndex columnIndex = model->index(index.row(), 0, QModelIndex());
    QString const key = QLatin1StringView("Favorites/") + model->data(columnIndex).toString();
    return QSettings().value(key).toDateTime();
}

}

#include "moc_MapThemeSortFilterProxyModel.cpp"
