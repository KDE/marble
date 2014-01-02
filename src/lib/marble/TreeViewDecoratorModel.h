//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014   Levente Kurusa <levex@linux.com>

#ifndef TREEVIEWDECORATORMODEL_H
#define TREEVIEWDECORATORMODEL_H

#include "GeoDataTreeModel.h"

#include <QSortFilterProxyModel>

namespace Marble
{

class TreeViewDecoratorModel : public QSortFilterProxyModel
{
public:
    explicit TreeViewDecoratorModel( QObject *parent = 0 );

protected:
    bool filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const;

};

}

#endif // TREEVIEWDECORATORMODEL_H
