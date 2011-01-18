//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010   Dennis Nienhüser <earthwings@gentoo.org>
//
//

#include "BookmarkManagerDialog.h"
#include "BookmarkManager.h"
#include "BookmarkManager_p.h"
#include "FileManager.h"
#include "GeoDataCoordinates.h"
#include "GeoDataExtendedData.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataTreeModel.h"
#include "GeoDataTypes.h"
#include "kdescendantsproxymodel.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "MarbleDataFacade.h"

#include <QtCore/QPointer>
#include <QtGui/QSortFilterProxyModel>

namespace Marble {

class BranchFilterModel : public QSortFilterProxyModel
{
public:
    explicit BranchFilterModel( QObject *parent = 0 );

    void setBranchIndex( KDescendantsProxyModel *sourceModel, const QModelIndex &index );

protected:
    bool filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const;

private:
    QPersistentModelIndex m_branchIndex;

    KDescendantsProxyModel* m_descendantsModel;
};

/*
 * The two list views use the model data like this:
 *
 *                                        (tree flatten)           (folder filter)
 *
 *                                     KDescendantsProxyModel => QSortFilterProxyModel => Folders View
 *                                   /                                   _______________________/
 * bookmarks.kml => GeoDataTreeModel <---------------------------\      /  user selected folder
 *                                   \                            \    V
 *                                     KDescendantsProxyModel => BranchFilterModel => QSortFilterProxyModel => Bookmarks View
 *
 *                                        (tree flatten)        (sel. folder filter)    (placemark filter)
 *
 */
class BookmarkManagerDialogPrivate {
public:
    BookmarkManagerDialog *m_parent;

    BookmarkManager *m_manager;

    BranchFilterModel *m_branchFilterModel;

    KDescendantsProxyModel *m_bookmarkFlatModel;

    QSortFilterProxyModel *m_bookmarkFilterModel;

    KDescendantsProxyModel *m_folderFlatModel;

    QSortFilterProxyModel *m_folderFilterModel;

    BookmarkManagerDialogPrivate( BookmarkManagerDialog* parent, BookmarkManager* manager );

    void initializeFoldersView( GeoDataTreeModel* treeModel );

    void initializeBookmarksView( GeoDataTreeModel* treeModel );

    void filterBookmarksByFolder( const QModelIndex &index );

    void updateButtonState();

    void renameFolder();

    void editBookmark();

    void discardChanges();
};

BranchFilterModel::BranchFilterModel( QObject *parent ) :
    QSortFilterProxyModel( parent ), m_descendantsModel( 0 )
{
    // nothing to do
}

void BranchFilterModel::setBranchIndex( KDescendantsProxyModel* descendantsModel, const QModelIndex &index )
{
    m_descendantsModel = descendantsModel;
    m_branchIndex = index;
    invalidateFilter();
}

bool BranchFilterModel::filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const
{
    if ( !m_descendantsModel || !m_branchIndex.isValid() ) {
        return true;
    }

    Q_ASSERT( m_descendantsModel == sourceModel() );
    QModelIndex const sourceIndex = sourceModel()->index( sourceRow, 0, sourceParent );
    QModelIndex proxyIndex = m_descendantsModel->mapToSource( sourceIndex );
    Q_ASSERT( proxyIndex.isValid() );

    while ( proxyIndex.isValid() && m_branchIndex != proxyIndex ) {
        proxyIndex = proxyIndex.parent();
    }

    return proxyIndex == m_branchIndex;
}

BookmarkManagerDialogPrivate::BookmarkManagerDialogPrivate( BookmarkManagerDialog* parent, BookmarkManager* manager ) :
    m_parent( parent ), m_manager( manager ), m_branchFilterModel( 0 ), m_bookmarkFlatModel( 0 ),
    m_bookmarkFilterModel( 0 ), m_folderFlatModel( 0 ), m_folderFilterModel( 0 )
{
    // nothing to do
}

void BookmarkManagerDialogPrivate::filterBookmarksByFolder( const QModelIndex &index )
{
    Q_ASSERT( index.isValid() );
    QModelIndex const flatModelIndex = m_folderFilterModel->mapToSource( index );
    Q_ASSERT( flatModelIndex.isValid() );
    QModelIndex const treeModelIndex = m_folderFlatModel->mapToSource( flatModelIndex );
    Q_ASSERT( treeModelIndex.isValid() );
    m_branchFilterModel->setBranchIndex( m_bookmarkFlatModel, treeModelIndex );
}

void BookmarkManagerDialogPrivate::updateButtonState()
{
    bool const hasFolderSelection = !m_parent->foldersListView->selectionModel()->selectedIndexes().isEmpty();
    m_parent->renameFolderButton->setEnabled( hasFolderSelection );

    bool const hasBookmarkSelection = !m_parent->bookmarksListView->selectionModel()->selectedIndexes().isEmpty();
    m_parent->editBookmarkButton->setEnabled( hasBookmarkSelection );
}

void BookmarkManagerDialogPrivate::renameFolder()
{
    QModelIndexList selection = m_parent->foldersListView->selectionModel()->selectedIndexes();
    if ( selection.size() == 1 ) {
        m_parent->foldersListView->openPersistentEditor( selection.first() );
        m_parent->foldersListView->setFocus();
    }
}

void BookmarkManagerDialogPrivate::editBookmark()
{
    QModelIndexList selection = m_parent->bookmarksListView->selectionModel()->selectedIndexes();
    if ( selection.size() == 1 ) {
        m_parent->bookmarksListView->openPersistentEditor( selection.first() );
        m_parent->bookmarksListView->setFocus();
    }
}

void BookmarkManagerDialogPrivate::discardChanges()
{
    m_manager->loadFile( "bookmarks/bookmarks.kml" );
}

void BookmarkManagerDialogPrivate::initializeFoldersView( GeoDataTreeModel* treeModel )
{
    // Set up the folders view chain
    m_folderFlatModel = new KDescendantsProxyModel( m_parent );
    m_folderFlatModel->setSourceModel( treeModel );

    m_folderFilterModel = new QSortFilterProxyModel( m_parent );
    m_folderFilterModel->setFilterKeyColumn( 1 );
    m_folderFilterModel->setFilterFixedString( GeoDataTypes::GeoDataFolderType );
    m_folderFilterModel->setSourceModel( m_folderFlatModel );

    m_parent->foldersListView->setModel( m_folderFilterModel );

    m_parent->connect( m_parent->foldersListView, SIGNAL( activated( QModelIndex ) ),
            m_parent, SLOT( filterBookmarksByFolder( QModelIndex ) ) );
    m_parent->connect( m_parent->foldersListView->selectionModel(),
                      SIGNAL( selectionChanged( QItemSelection, QItemSelection ) ),
                      m_parent, SLOT( updateButtonState() ) );
    m_parent->connect( m_parent->renameFolderButton, SIGNAL( clicked( bool ) ),
                       m_parent, SLOT( renameFolder() ) );
}

void BookmarkManagerDialogPrivate::initializeBookmarksView( GeoDataTreeModel* treeModel )
{
    // Set up the bookmark view chain
    m_bookmarkFlatModel = new KDescendantsProxyModel( m_parent );
    m_bookmarkFlatModel->setSourceModel( treeModel );

    m_branchFilterModel = new BranchFilterModel( m_parent );
    m_branchFilterModel->setSourceModel( m_bookmarkFlatModel );

    m_bookmarkFilterModel = new QSortFilterProxyModel( m_parent );
    m_bookmarkFilterModel->setFilterKeyColumn( 1 );
    m_bookmarkFilterModel->setFilterFixedString( GeoDataTypes::GeoDataPlacemarkType );
    m_bookmarkFilterModel->setSourceModel( m_branchFilterModel );

    m_parent->bookmarksListView->setModel( m_bookmarkFilterModel );

    m_parent->connect( m_parent->bookmarksListView->selectionModel(),
                      SIGNAL( selectionChanged( QItemSelection, QItemSelection ) ),
                      m_parent, SLOT( updateButtonState() ) );
    m_parent->connect( m_parent->editBookmarkButton, SIGNAL( clicked( bool ) ),
                       m_parent, SLOT( editBookmark() ) );
}

BookmarkManagerDialog::BookmarkManagerDialog( MarbleModel* model, QWidget *parent )
    : QDialog( parent ),
      d( new BookmarkManagerDialogPrivate( this, model->bookmarkManager() ) )
{
    setupUi( this );

    GeoDataTreeModel* treeModel = new GeoDataTreeModel( this );
    treeModel->setRootDocument( model->bookmarkManager()->d->bookmarkDocument() );

    d->initializeFoldersView( treeModel );
    d->initializeBookmarksView( treeModel );
    d->updateButtonState();

    connect( this, SIGNAL( accepted() ), SLOT( saveBookmarks() ) );
    connect( this, SIGNAL( rejected() ), SLOT( discardChanges() ) );
}

BookmarkManagerDialog::~BookmarkManagerDialog()
{
    delete d;
}

void BookmarkManagerDialog::saveBookmarks()
{
    d->m_manager->updateBookmarkFile();
}

}

#include "BookmarkManagerDialog.moc"
