//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014   Levente Kurusa <levex@linux.com>

#include "TreeViewDecoratorModel.h"
#include "MarbleDebug.h"
#include "GeoDataFolder.h"
#include "GeoDataTypes.h"
#include "GeoDataObject.h"
#include "GeoDataContainer.h"
#include "GeoDataStyle.h"
#include "GeoDataListStyle.h"
#include "GeoDataItemIcon.h"
#include "MarblePlacemarkModel.h"

#include <QImage>

namespace Marble
{

TreeViewDecoratorModel::TreeViewDecoratorModel( QObject *parent ) :
    QSortFilterProxyModel( parent )
{
    // nothing to do
}

bool TreeViewDecoratorModel::filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const
{
    QModelIndex rowIndex = sourceModel()->index( sourceRow, 0, sourceParent );

    GeoDataObject* object = qvariant_cast<GeoDataObject*>( rowIndex.data( MarblePlacemarkModel::ObjectPointerRole ) );
    GeoDataObject* parent = object->parent();
    if ( parent->nodeType() == GeoDataTypes::GeoDataFolderType ||
         parent->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
        GeoDataContainer *container = static_cast<GeoDataContainer *>( parent );
        if ( container->style()->listStyle().listItemType() == GeoDataListStyle::CheckHideChildren ) {
            return false;
        }
    }

    return QSortFilterProxyModel::filterAcceptsRow( sourceRow, sourceParent );
}

QVariant TreeViewDecoratorModel::data( const QModelIndex &proxyIndex, int role) const
{
    if ( role != Qt::DecorationRole || proxyIndex.column() != 0 ) {
        return QSortFilterProxyModel::data(proxyIndex, role);
    }

    GeoDataObject *object = qvariant_cast<GeoDataObject *>( QSortFilterProxyModel::data(proxyIndex, MarblePlacemarkModel::ObjectPointerRole));
    if ( !object ) {
        return QSortFilterProxyModel::data(proxyIndex, role);
    }

    if ( object->nodeType() != GeoDataTypes::GeoDataFolderType ) {
        return QSortFilterProxyModel::data(proxyIndex, role);
    }

    GeoDataFolder *folder = static_cast<GeoDataFolder *>( object );

    bool const expandedState = m_expandedRows.contains( QPersistentModelIndex( proxyIndex ) );

    foreach (GeoDataItemIcon *icon, folder->style()->listStyle().itemIconList()) {
        if ( ! expandedState ) {
            if ( icon->state() == GeoDataItemIcon::Closed ) {
                return icon->icon();
            }
        } else {
            if ( icon->state() == GeoDataItemIcon::Open ) {
                return icon->icon();
            }
        }
    }

    return QSortFilterProxyModel::data(proxyIndex, role);
}

void TreeViewDecoratorModel::trackExpandedState( const QModelIndex &index )
{
    m_expandedRows << QPersistentModelIndex( index );
}

void TreeViewDecoratorModel::trackCollapsedState( const QModelIndex &index )
{
    m_expandedRows.removeAll( QPersistentModelIndex( index ));
}

}
#include "moc_TreeViewDecoratorModel.cpp"

