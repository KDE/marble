#include "MapThemeSortFilterProxyModel.h"
#include <QtCore/QString>

MapThemeSortFilterProxyModel::MapThemeSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool MapThemeSortFilterProxyModel::lessThan ( const QModelIndex & left, const QModelIndex & right ) const
{
    QString leftData = sourceModel()->data( left ).toString();
    QString rightData = sourceModel()->data( right ).toString();
    if ( leftData == "Atlas" or
         leftData == "Satellite View" or
         leftData == "OpenStreetMap" )
        return true;
    else if ( rightData == "Atlas" or
         rightData == "Satellite View" or
         rightData == "OpenStreetMap" )
        return false;
    else
        return QSortFilterProxyModel::lessThan( left, right);
}

#include "MapThemeSortFilterProxyModel.moc"
