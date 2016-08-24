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

#include <QString>
#include <QModelIndex>
#include <QDateTime>
#include <QSettings>

/* TRANSLATOR Marble::MapThemeSortFilterProxyModel */

namespace Marble {

MapThemeSortFilterProxyModel::MapThemeSortFilterProxyModel( QObject *parent )
    : QSortFilterProxyModel( parent )
{
    // nothing to do
}

bool MapThemeSortFilterProxyModel::lessThan ( const QModelIndex & left, const QModelIndex & right ) const
{
    if( isFavorite( left ) ) {
        if( !isFavorite( right ) ) {
            return true;
        }
    }
    else {
        if( isFavorite( right ) ) {
            return false;
        }
    }
    return sourceModel()->data( left ).toString() < sourceModel()->data( right ).toString();;
}

bool MapThemeSortFilterProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const
 {
     QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
     return (sourceModel()->data( index, Qt::UserRole + 1 ).toString().contains( filterRegExp() ) );
 }
 
bool MapThemeSortFilterProxyModel::isFavorite( const QModelIndex& index )
{
    const QAbstractItemModel *model = index.model();
    QModelIndex columnIndex = model->index( index.row(), 0, QModelIndex() );
    QString const key = QLatin1String("Favorites/") + model->data(columnIndex).toString();
    return QSettings().contains( key );
}

QDateTime MapThemeSortFilterProxyModel::favoriteDateTime( const QModelIndex& index )
{
    const QAbstractItemModel *model = index.model();
    QModelIndex columnIndex = model->index( index.row(), 0, QModelIndex() );
    QString const key = QLatin1String("Favorites/") + model->data(columnIndex).toString();
    return QSettings().value( key ).toDateTime();
}

}

#include "moc_MapThemeSortFilterProxyModel.cpp"
