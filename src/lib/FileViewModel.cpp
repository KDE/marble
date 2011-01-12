//
// This file is part of the Marble Virtual Globe.
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
#include "GeoDataDocument.h"
#include "FileManager.h"

#include <QtGui/QItemSelectionModel>

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
            const GeoDataDocument *document = m_manager->at( row );

            if ( role == Qt::CheckStateRole ) {
                return document->isVisible() ? Qt::Checked : Qt::Unchecked;
            }
            if ( role == Qt::DisplayRole ) {
                return document->name();
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

                GeoDataDocument *document = m_manager->at( row );
                bool newValue = value.toBool ();

                if ( document->isVisible() != newValue ) {

                    document->setVisible( newValue );
                    emit dataChanged( index, index );
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
}

void FileViewModel::remove( int index )
{
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
}

#include "FileViewModel.moc"
