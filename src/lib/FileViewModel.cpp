#include "FileViewModel.h"
#include "AbstractFileViewItem.h"

FileViewModel::FileViewModel( QObject* parent ) :
    QAbstractListModel( parent )
{
}

int FileViewModel::rowCount( const QModelIndex & parent ) const
{
    //TODO
    return 0;
}

QVariant FileViewModel::data( const QModelIndex & index, int role ) const
{
    //TODO
    return QVariant();
}

Qt::ItemFlags FileViewModel::flags( const QModelIndex & index ) const
{
    return Qt::ItemFlags( Qt::ItemIsUserCheckable |
                          Qt::ItemIsEnabled |
                          Qt::ItemIsSelectable );
}

void FileViewModel::setSelectedIndex( const QModelIndex& index )
{
    m_selectedIndex = index;
}

void FileViewModel::saveFile()
{
    if ( m_selectedIndex.isValid() ) {
        AbstractFileViewItem* item = ( AbstractFileViewItem* ) m_selectedIndex.internalPointer();

        if ( item != 0 ) {
            item->saveFile();
        }
    }
}

void FileViewModel::closeFile()
{
    if ( m_selectedIndex.isValid() ) {
        AbstractFileViewItem* item = ( AbstractFileViewItem* ) m_selectedIndex.internalPointer();

        if (  item != 0 ) {
            item->closeFile();

            /*
             *TODO
             *remove this item from model
             *and emit layoutChanged
             */
        }
    }
}

#include "FileViewModel.moc"
