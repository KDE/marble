//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

// Own
#include "FileViewModel.h"

#include <QtGui/QItemSelectionModel>

#include "AbstractFileViewItem.h"
#include "FileManager.h"

using namespace Marble;

FileViewModel::FileViewModel( QObject* parent ) :
    QAbstractListModel( parent ),
    m_selectionModel(new QItemSelectionModel(this))
{
}

FileViewModel::~FileViewModel()
{
}

int FileViewModel::rowCount( const QModelIndex & parent ) const
{
    Q_UNUSED( parent );

    return m_manager->size();
}

QVariant FileViewModel::data( const QModelIndex & index, int role ) const
{
    if ( !index.isValid() ) {
        return QVariant();
    }

    int row = index.row();

    if ( row < m_manager->size() ) {
        if ( index.column() == 0 ) {
            const AbstractFileViewItem& item = *m_manager->at( row );

            if ( role == Qt::CheckStateRole ) {
                return item.isShown () ? Qt::Checked : Qt::Unchecked;
            }
            if ( role == Qt::DisplayRole ) {
                return item.name();
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

bool FileViewModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
    if ( !index.isValid() ) {
        return false;
    }

    int row = index.row();

    if ( row < m_manager->size() ) {
        if ( index.column() == 0 ) {
            if ( role == Qt::CheckStateRole ) {

                AbstractFileViewItem& item = *m_manager->at( row );
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

void FileViewModel::saveFile()
{
    if ( m_selectionModel->hasSelection() )
    {
        m_manager->saveFile( m_selectionModel->selectedRows().first().row() );
    }
}

void FileViewModel::closeFile()
{
    if ( m_selectionModel->hasSelection() )
    {
        m_manager->closeFile( m_selectionModel->selectedRows().first().row() );
    }
}
void FileViewModel::setFileManager( FileManager *fileManager)
{
    disconnect( this, SLOT(append(int)) );
    disconnect( this, SLOT(remove(int)) );
    m_manager = fileManager;
    connect (m_manager, SIGNAL(fileAdded(int)),
             this, SLOT(append(int)));
    connect (m_manager, SIGNAL(fileRemoved(int)),
             this, SLOT(remove(int)));
}

QItemSelectionModel * FileViewModel::selectionModel()
{
    return m_selectionModel;
}

void FileViewModel::append( int order )
{
    beginInsertRows(QModelIndex(), order, order);
    endInsertRows();
    emit modelChanged();
}

void FileViewModel::remove( int index )
{
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
    emit modelChanged();
}

#include "FileViewModel.moc"
