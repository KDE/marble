//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010   Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2012   Thibaut Gridel <tgridel@free.fr>

#ifndef BRANCHFILTERPROXYMODEL_H
#define BRANCHFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

namespace Marble
{

class GeoDataTreeModel;

class BranchFilterProxyModel : public QSortFilterProxyModel
{
public:
    explicit BranchFilterProxyModel( QObject *parent = 0 );

    void setBranchIndex( GeoDataTreeModel *sourceModel, const QModelIndex &index );

protected:
    bool filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const;

private:
    GeoDataTreeModel *m_treeModel;
    QPersistentModelIndex m_branchIndex;

};

}

#endif // BRANCHFILTERPROXYMODEL_H
