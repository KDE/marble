//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//

// Own
#include "FileViewModel.h"

// Other
#include "AbstractFileViewItem.h"

using namespace Marble;

FileViewModel::FileViewModel( QObject* parent ) :
    QAbstractListModel( parent )
{
}

FileViewModel::~FileViewModel()
{
    foreach ( AbstractFileViewItem* item, m_itemList ) {
        delete item;
    }
}

int FileViewModel::rowCount( const QModelIndex & parent ) const
{
    Q_UNUSED( parent );

    return m_itemList.count();
}

QVariant FileViewModel::data( const QModelIndex & index, int role ) const
{
    if ( !index.isValid() ) {
        return QVariant();
    }

    int row = index.row();

    if ( row < m_itemList.count() ) {
        if ( index.column() == 0 ) {
            const AbstractFileViewItem& item = *m_itemList.at( row );

            if ( role == Qt::DisplayRole ) {
                return item.data ();
            }
            else if ( role == Qt::CheckStateRole ) {
                return item.isShown () ? Qt::Checked : Qt::Unchecked;
            }
            else if ( role == AbstractFileViewItem::FilePointerRole ) {
                return item.data( AbstractFileViewItem::FilePointerRole );
            }
        }
    }

    return QVariant();
}

Qt::ItemFlags FileViewModel::flags( const QModelIndex & index ) const
{
    Q_UNUSED( index );

    return Qt::ItemFlags( Qt::ItemIsUserCheckable |
                          Qt::ItemIsEnabled |
                          Qt::ItemIsSelectable );
}

bool FileViewModel::setData (const QModelIndex& index, const QVariant& value, int role )
{
    if ( !index.isValid() ) {
        return false;
    }

    int row = index.row();

    if ( row < m_itemList.count() ) {
        if ( index.column() == 0 ) {
            if ( role == Qt::CheckStateRole ) {

                AbstractFileViewItem& item = *m_itemList.at( row );
                bool newValue = value.toBool ();

                if ( item.isShown() != newValue ) {

                    item.setShown( newValue );
                    emit dataChanged( index, index );
                    emit modelChanged();
                    return true;
                }
            }
        }
    }

    return false;
}

void FileViewModel::setSelectedIndex( const QModelIndex& index )
{
    m_selectedIndex = index;
}

void FileViewModel::append ( AbstractFileViewItem* item )
{
    m_itemList.append( item );
    emit layoutChanged();
    emit modelChanged();
}

void FileViewModel::saveFile()
{
    if ( m_selectedIndex.isValid() ) {

        int row = m_selectedIndex.row();

        if ( row < m_itemList.count() ) {
            if ( m_selectedIndex.column() == 0 ) {

                AbstractFileViewItem& item = *m_itemList.at( row );
                item.saveFile();

            }
        }
    }
}

void FileViewModel::closeFile()
{
    if ( m_selectedIndex.isValid() ) {

        int row = m_selectedIndex.row();

        if ( row < m_itemList.count() ) {
            if ( m_selectedIndex.column() == 0 ) {

                AbstractFileViewItem& item = *m_itemList.at( row );
                item.closeFile();

                delete &item;
                m_itemList.removeAt( row );

                emit layoutChanged();
                emit modelChanged();
            }
        }
    }
}

#include "FileViewModel.moc"
