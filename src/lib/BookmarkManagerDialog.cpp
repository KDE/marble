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
#include "GeoDataDocument.h"
#include "GeoDataExtendedData.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataTreeModel.h"
#include "GeoDataTypes.h"
#include "GeoDataParser.h"
#include "GeoWriter.h"
#include "kdescendantsproxymodel.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "MarbleDataFacade.h"

#include <QtCore/QPointer>
#include <QtCore/QFile>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

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

    GeoDataTreeModel* m_treeModel;

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

    GeoDataDocument* openDocument( const QString &file ) const;

    void insertBookmark( GeoDataDocument* document, const QString &folderName, GeoDataPlacemark* placemark ) const;
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
    m_parent( parent ), m_manager( manager ), m_treeModel( 0 ), m_branchFilterModel( 0 ), m_bookmarkFlatModel( 0 ),
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

void BookmarkManagerDialogPrivate::insertBookmark( GeoDataDocument *document, const QString &folderName, GeoDataPlacemark *placemark ) const
{
    foreach( GeoDataFolder* existingFolder, document->folderList() ) {
        if ( existingFolder->name() == folderName ) {
            mDebug() << "Using existing folder " << existingFolder->name() << " to host new bookmark " << placemark->name();
            existingFolder->append( placemark );
            return;
        }
    }

    mDebug() << "Creating new folder " << folderName << " to host " << placemark->name();
    GeoDataFolder* folder = new GeoDataFolder;
    folder->setName( folderName );
    folder->append( placemark );
    document->append( folder );
}

BookmarkManagerDialog::BookmarkManagerDialog( MarbleModel* model, QWidget *parent )
    : QDialog( parent ),
      d( new BookmarkManagerDialogPrivate( this, model->bookmarkManager() ) )
{
    setupUi( this );

    d->m_treeModel = new GeoDataTreeModel( this );
    d->m_treeModel->setRootDocument( model->bookmarkManager()->d->bookmarkDocument() );

    d->initializeFoldersView( d->m_treeModel );
    d->initializeBookmarksView( d->m_treeModel );
    d->updateButtonState();

    connect( this, SIGNAL( accepted() ), SLOT( saveBookmarks() ) );
    connect( this, SIGNAL( rejected() ), SLOT( discardChanges() ) );
    connect( exportButton, SIGNAL( clicked() ), this, SLOT( exportBookmarks() ) );
    connect( importButton, SIGNAL( clicked() ), this, SLOT( importBookmarks() ) );
}

BookmarkManagerDialog::~BookmarkManagerDialog()
{
    delete d;
}

void BookmarkManagerDialog::saveBookmarks()
{
    d->m_manager->updateBookmarkFile();
}

void BookmarkManagerDialog::exportBookmarks()
{
    QString fileName = QFileDialog::getSaveFileName( this, tr( "Export Bookmarks" ), // krazy:exclude=qclasses
                       QDir::homePath(), tr( "KML files (*.kml)" ) );

    if ( !fileName.isEmpty() ) {
        QFile file( fileName );
        GeoWriter writer;
        writer.setDocumentType( "http://earth.google.com/kml/2.2" );

        if ( !file.open( QIODevice::ReadWrite ) || !writer.write( &file, d->m_manager->d->bookmarkDocument() ) ) {
            mDebug() << "Could not write the bookmarks file" << fileName;
            QString const text = tr( "Unable to save bookmarks. Please check that the file is writable." );
            QMessageBox::warning( this, tr( "Bookmark Export - Marble" ), text );
        }
    }
}

GeoDataDocument* BookmarkManagerDialogPrivate::openDocument( const QString &fileName ) const
{
    GeoDataParser parser( GeoData_KML );
    QFile file( fileName );

    if ( !file.exists() ) {
        return 0;
    }

    if ( !file.open( QIODevice::ReadOnly ) || !parser.read( &file ) ) {
        mDebug() << "Could not open/parse file" << fileName;
        return 0;
    }

    GeoDataDocument *result = dynamic_cast<GeoDataDocument*>( parser.releaseDocument() );
    if ( !result ) {
        return 0;
    }

    foreach( GeoDataFolder* folder, result->folderList() ) {
        foreach( GeoDataPlacemark* placemark, folder->placemarkList() ) {
            placemark->setVisualCategory( GeoDataFeature::Bookmark );
        }
    }

    return result;
}

void BookmarkManagerDialog::importBookmarks()
{
    QString const file = QFileDialog::getOpenFileName( this, tr( "Import Bookmarks - Marble" ),
                            QDir::homePath(), tr( "KML Files (*.kml)" ) );
    if ( file.isEmpty() ) {
        return;
    }

    GeoDataDocument* import = d->openDocument( file );
    if ( !import ) {
        QString const text = tr( "The file %1 cannot be opened as a KML file." ).arg( file );
        QMessageBox::warning( this, tr( "Bookmark Import - Marble" ), text );
        return;
    }

    GeoDataDocument* current = d->m_manager->d->bookmarkDocument();
    d->m_treeModel->setRootDocument( 0 );

    bool replaceAll = false;
    bool skipAll = false;
    foreach( GeoDataFolder* newFolder, import->folderList() ) {
        foreach( GeoDataPlacemark* newPlacemark, newFolder->placemarkList() ) {
            bool added = skipAll;
            foreach( GeoDataFolder* existingFolder, current->folderList() ) {
                for( int i=0; i<existingFolder->size() && !added; ++i ) {
                    GeoDataPlacemark* existingPlacemark = dynamic_cast<GeoDataPlacemark*>( existingFolder->child( i ) );
                    if ( existingPlacemark && existingPlacemark->coordinate() == newPlacemark->coordinate() ) {

                        if ( skipAll ) {
                            continue;
                        }

                        // Avoid message boxes for equal bookmarks, just skip them
                        if ( existingPlacemark->name() == newPlacemark->name() &&
                             existingPlacemark->description() == newPlacemark->description() ) {
                            added = true;
                            continue;
                        }

                        QMessageBox messageBox( this );
                        QString const intro = tr( "The file contains a bookmark that already exists among your Bookmarks." );
                        QString const newBookmark = tr( "Imported bookmark" );
                        QString const existingBookmark = tr( "Existing bookmark" );
                        QString const question = tr( "Do you want to replace the existing bookmark with the imported one?" );
                        QString html = "<p>%1</p><table><tr><td>%2</td><td><b>%3 / %4</b></td></tr>";
                        html += "<tr><td>%5</td><td><b>%6 / %7</b></td></tr></table><p>%8</p>";
                        html = html.arg( intro ).arg( existingBookmark ).arg( existingFolder->name() );
                        html = html.arg( existingPlacemark->name() ).arg( newBookmark ).arg( newFolder->name() );
                        html = html.arg( newPlacemark->name() ).arg( question );
                        messageBox.setText( html );

                        QAbstractButton *replaceButton    = messageBox.addButton(tr( "Replace" ),     QMessageBox::ActionRole );
                        QAbstractButton *replaceAllButton = messageBox.addButton(tr( "Replace All" ), QMessageBox::ActionRole );
                        QAbstractButton *skipButton       = messageBox.addButton(tr( "Skip" ),        QMessageBox::ActionRole );
                        QAbstractButton *skipAllButton    = messageBox.addButton(tr( "Skip All" ),    QMessageBox::ActionRole );
                                                            messageBox.addButton(tr( "Cancel" ),      QMessageBox::RejectRole );
                        messageBox.setIcon( QMessageBox::Question );

                        if ( !replaceAll ) {
                            messageBox.exec();
                        }
                        if ( messageBox.clickedButton() == replaceAllButton ) {
                            replaceAll = true;
                        } else if ( messageBox.clickedButton() == skipAllButton ) {
                            skipAll = true;
                            added = true;
                        } else if ( messageBox.clickedButton() == skipButton ) {
                            added = true;
                            continue;
                        } else if ( messageBox.clickedButton() != replaceButton ) {
                            d->m_treeModel->setRootDocument( current );
                            return;
                        }

                        if ( messageBox.clickedButton() == replaceButton || replaceAll ) {
                            existingFolder->remove( i );
                            d->insertBookmark( current, newFolder->name(), newPlacemark );
                            mDebug() << "Placemark " << newPlacemark->name() << " replaces " << existingPlacemark->name();
                            added = true;
                            delete existingPlacemark;
                            break;
                        }
                    }
                }
            }

            if ( !added ) {
                d->insertBookmark( current, newFolder->name(), newPlacemark );
            }
        }
    }

    d->m_treeModel->setRootDocument( current );
}

}

#include "BookmarkManagerDialog.moc"
