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
#include <QtCore/QModelIndex>
#include <QtCore/QDateTime>
#include <QtCore/QSettings>

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
    if( isFavorite( left ) && isFavorite( right ) ) {
        if( favoriteDateTime( left ) != favoriteDateTime( right ) ) {
            return favoriteDateTime( left ) > favoriteDateTime( right );
        }
    }
    return sourceModel()->data( left ).toString() < sourceModel()->data( right ).toString();;
}

bool MapThemeSortFilterProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const
 {
     QModelIndex index = sourceModel()->index(sourceRow, 1, sourceParent);
     return (sourceModel()->data( index ).toString().contains( filterRegExp() ) );
 }
 
bool MapThemeSortFilterProxyModel::isFavorite( const QModelIndex& index ) const
{
    const QAbstractItemModel *model = index.model();
    QModelIndex columnIndex = model->index( index.row(), 0, QModelIndex() );
    QString const key = "Favorites/" + model->data( columnIndex ).toString();
    return QSettings( "kde.org", "Marble Desktop Globe" ).contains( key );
}

QDateTime MapThemeSortFilterProxyModel::favoriteDateTime( const QModelIndex& index ) const
{
    const QAbstractItemModel *model = index.model();
    QModelIndex columnIndex = model->index( index.row(), 0, QModelIndex() );
    QString const key = "Favorites/" + model->data( columnIndex ).toString();
    return QSettings( "kde.org", "Marble Desktop Globe" ).value( key ).toDateTime();
}

}

#include "MapThemeSortFilterProxyModel.moc"
