// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2012 Thibaut Gridel <tgridel@free.fr>

#ifndef BRANCHFILTERPROXYMODEL_H
#define BRANCHFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

namespace Marble
{

class GeoDataTreeModel;

class BranchFilterProxyModel : public QSortFilterProxyModel
{
public:
    explicit BranchFilterProxyModel(QObject *parent = nullptr);

    void setBranchIndex(GeoDataTreeModel *sourceModel, const QModelIndex &index);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    GeoDataTreeModel *m_treeModel;
    QPersistentModelIndex m_branchIndex;
};

}

#endif // BRANCHFILTERPROXYMODEL_H
