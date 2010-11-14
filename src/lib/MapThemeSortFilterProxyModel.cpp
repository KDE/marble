//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Claudiu Covaci <claudiu.covaci@gmail.com>
// Copyright 2009      Torsten Rahn <tackat@kde.org>
//

#include "MapThemeSortFilterProxyModel.h"
#include <QtCore/QString>

using namespace Marble;

MapThemeSortFilterProxyModel::MapThemeSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool MapThemeSortFilterProxyModel::lessThan ( const QModelIndex & left, const QModelIndex & right ) const
{
    QString leftData = sourceModel()->data( left ).toString();
    QString rightData = sourceModel()->data( right ).toString();
    if ( leftData == tr("Atlas") ||
         leftData == tr("Satellite View") ||
         leftData == tr("OpenStreetMap") )
        return true;
    else if ( rightData == tr("Atlas") ||
         rightData == tr("Satellite View") ||
         rightData == tr("OpenStreetMap") )
        return false;
    else
        return QSortFilterProxyModel::lessThan( left, right);
}

bool MapThemeSortFilterProxyModel::filterAcceptsRow(int sourceRow,
         const QModelIndex &sourceParent) const
 {
     QModelIndex index = sourceModel()->index(sourceRow, 1, sourceParent);

     return (sourceModel()->data(index).toString().contains(filterRegExp()));
            
 }

#include "MapThemeSortFilterProxyModel.moc"
