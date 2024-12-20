// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Levente Kurusa <levex@linux.com>

#ifndef TREEVIEWDECORATORMODEL_H
#define TREEVIEWDECORATORMODEL_H

#include <QSortFilterProxyModel>

namespace Marble
{

class TreeViewDecoratorModel : public QSortFilterProxyModel
{
    Q_OBJECT

public Q_SLOTS:
    void trackExpandedState(const QModelIndex &index);
    void trackCollapsedState(const QModelIndex &index);

public:
    explicit TreeViewDecoratorModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QList<QPersistentModelIndex> m_expandedRows;
};

}

#endif // TREEVIEWDECORATORMODEL_H
