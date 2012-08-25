//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "SatellitesConfigModel.h"

#include "SatellitesConfigNodeItem.h"
#include "SatellitesConfigLeafItem.h"
#include "MarbleDebug.h"

namespace Marble {

SatellitesConfigModel::SatellitesConfigModel( QObject *parent )
    : QAbstractItemModel( parent ),
      m_rootItem( new SatellitesConfigNodeItem( "" ) )
{
}

void SatellitesConfigModel::loadSettings( QHash<QString, QVariant> settings)
{
    m_rootItem->loadSettings( settings );
}

void SatellitesConfigModel::appendChild( SatellitesConfigAbstractItem *child )
{
    m_rootItem->appendChild( child );
}

QVariant SatellitesConfigModel::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() ) {
        return QVariant();
    }

    SatellitesConfigAbstractItem *item = static_cast<SatellitesConfigAbstractItem *>( index.internalPointer() );
    return item->data( index.column(), role );
}

bool SatellitesConfigModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    SatellitesConfigAbstractItem *item = static_cast<SatellitesConfigAbstractItem *>( index.internalPointer() );

    bool success = item->setData( index.column(), role, value );

    if ( success ) {
        QModelIndex parentCellIndex = this->index( index.parent().row(), index.column(), index.parent().parent() );
        emit dataChanged( parentCellIndex, parentCellIndex );
    }

    return success;
}

int SatellitesConfigModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent )
    //TODO: enable second column
    return 1;
}

int SatellitesConfigModel::rowCount( const QModelIndex &parent ) const
{
    if ( parent.column() > 0 ) {
        return 0;
    }

    SatellitesConfigAbstractItem *parentItem = 0;
    if ( !parent.isValid() ) {
        parentItem = m_rootItem;
    } else {
        parentItem = static_cast<SatellitesConfigAbstractItem *>( parent.internalPointer() );
    }

    return parentItem->childrenCount();
}

QModelIndex SatellitesConfigModel::parent( const QModelIndex &child ) const
{
    if ( !child.isValid() ) {
        return QModelIndex();
    }

    SatellitesConfigAbstractItem *childItem = static_cast<SatellitesConfigAbstractItem *>( child.internalPointer() );
    SatellitesConfigAbstractItem *parentItem = childItem->parent();
    if ( parentItem == m_rootItem ) {
        return QModelIndex();
    }

    return createIndex( parentItem->row(), 0, parentItem);
}

QModelIndex SatellitesConfigModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( !hasIndex( row, column, parent ) ) {
        return QModelIndex();
    }

    SatellitesConfigAbstractItem *parentItem = 0;
    if ( !parent.isValid() ) {
        parentItem = m_rootItem;
    } else {
        parentItem = static_cast<SatellitesConfigAbstractItem *>( parent.internalPointer() );
    }

    SatellitesConfigAbstractItem *childItem = parentItem->childAt( row );

    if ( !childItem ) {
        return QModelIndex();
    }

    return createIndex( row, column, childItem );    
}

QVariant SatellitesConfigModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation != Qt::Horizontal || role != Qt::DisplayRole ) {
        return QVariant();
    }

    switch (section) {
        case 0: {
            return QVariant( tr( "Category" ) );
        }
        default: {
            return QVariant();
        }
    }
}

Qt::ItemFlags SatellitesConfigModel::flags( const QModelIndex &index ) const
{
    if ( !index.isValid() ) {
        return 0;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}

} // namespace Marble

