// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2012 Thibaut Gridel <tgridel@free.fr>

#include "BranchFilterProxyModel.h"

#include "GeoDataContainer.h"
#include "GeoDataTreeModel.h"
#include "MarblePlacemarkModel.h"

namespace Marble
{

BranchFilterProxyModel::BranchFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_treeModel(nullptr)
{
    // nothing to do
}

/// sets the folder index for which we want to see bookmarks
void BranchFilterProxyModel::setBranchIndex(GeoDataTreeModel *treeModel, const QModelIndex &index)
{
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.model() == treeModel);
    m_treeModel = treeModel;
    m_branchIndex = index;
    invalidateFilter();
}

/// determines if such row should be filtered.
/// Beware, all parents of our folder must be accepted as well
bool BranchFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (!m_treeModel || !m_branchIndex.isValid()) {
        return true;
    }

    Q_ASSERT(m_treeModel == sourceModel());
    if (sourceParent.isValid()) {
        Q_ASSERT(sourceParent.model() == m_treeModel);
    }
    QModelIndex rowIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    Q_ASSERT(rowIndex.isValid());

    // return true for all non folder children of m_branchIndex
    if (sourceParent == m_branchIndex) {
        auto obj = qvariant_cast<GeoDataObject *>(rowIndex.data(MarblePlacemarkModel::ObjectPointerRole));
        return !dynamic_cast<const GeoDataContainer *>(obj);
    }

    // return true if rowIndex is a parent of m_branchIndex
    QModelIndex tmpIndex = m_branchIndex;
    while (tmpIndex.isValid() && tmpIndex != rowIndex) {
        tmpIndex = tmpIndex.parent();
    }
    return tmpIndex == rowIndex;
}

}
