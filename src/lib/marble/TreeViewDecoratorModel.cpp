// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Levente Kurusa <levex@linux.com>

#include "TreeViewDecoratorModel.h"
#include "GeoDataContainer.h"
#include "GeoDataFolder.h"
#include "GeoDataGeometry.h"
#include "GeoDataItemIcon.h"
#include "GeoDataListStyle.h"
#include "GeoDataObject.h"
#include "GeoDataStyle.h"
#include "MarbleDebug.h"
#include "MarblePlacemarkModel.h"

#include <QImage>

namespace Marble
{

TreeViewDecoratorModel::TreeViewDecoratorModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    // nothing to do
}

bool TreeViewDecoratorModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex rowIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    const GeoDataObject *object = qvariant_cast<GeoDataObject *>(rowIndex.data(MarblePlacemarkModel::ObjectPointerRole));
    const GeoDataObject *parent = object->parent();
    if (const auto container = dynamic_cast<const GeoDataContainer *>(parent)) {
        if (container->style()->listStyle().listItemType() == GeoDataListStyle::CheckHideChildren) {
            return false;
        }
    }

    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

QVariant TreeViewDecoratorModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (role != Qt::DecorationRole || proxyIndex.column() != 0) {
        if (proxyIndex.column() == 1) {
            return QSortFilterProxyModel::data(proxyIndex, role).toString().remove(QStringLiteral("GeoData"));
        } else {
            return QSortFilterProxyModel::data(proxyIndex, role);
        }
    }

    auto object = qvariant_cast<GeoDataObject *>(QSortFilterProxyModel::data(proxyIndex, MarblePlacemarkModel::ObjectPointerRole));
    if (!object) {
        return QSortFilterProxyModel::data(proxyIndex, role);
    }

    auto folder = dynamic_cast<GeoDataFolder *>(object);

    if (folder) {
        bool const expandedState = m_expandedRows.contains(QPersistentModelIndex(proxyIndex));

        for (GeoDataItemIcon *icon : folder->style()->listStyle().itemIconList()) {
            if (!expandedState) {
                if (icon->state() == GeoDataItemIcon::Closed) {
                    return icon->icon();
                }
            } else {
                if (icon->state() == GeoDataItemIcon::Open) {
                    return icon->icon();
                }
            }
        }
    }

    return QSortFilterProxyModel::data(proxyIndex, role);
}

void TreeViewDecoratorModel::trackExpandedState(const QModelIndex &index)
{
    m_expandedRows << QPersistentModelIndex(index);
}

void TreeViewDecoratorModel::trackCollapsedState(const QModelIndex &index)
{
    m_expandedRows.removeAll(QPersistentModelIndex(index));
}

}
#include "moc_TreeViewDecoratorModel.cpp"
