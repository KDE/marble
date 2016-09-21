//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010   Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2012   Thibaut Gridel <tgridel@free.fr>
//

#include "BookmarkManagerDialog.h"
#include "BookmarkManager.h"
#include "BookmarkManager_p.h"
#include "BranchFilterProxyModel.h"
#include "EditBookmarkDialog.h"
#include "FileManager.h"
#include "GeoDataCoordinates.h"
#include "GeoDataDocument.h"
#include "GeoDataLookAt.h"
#include "GeoDataExtendedData.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataTreeModel.h"
#include "GeoDataTypes.h"
#include "GeoDataDocumentWriter.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "NewBookmarkFolderDialog.h"
#include "MarblePlacemarkModel.h"
#include <KmlElementDictionary.h>

#include <QPointer>
#include <QFile>
#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QMessageBox>

namespace Marble {

/*
 * The two list views use the model data like this:
 *
 *                                     (folder filter)
 *
 *                                     QSortFilterProxyModel => Folders View
 *                                   /         |
 * bookmarks.kml => GeoDataTreeModel           |  current folder sets filter
 *                                   \        \ /
 *                                     BranchFilterModel => Bookmarks View
 *
 *                                     (placemark filter)   (placemark list)
 *
 */
class BookmarkManagerDialogPrivate {
public:
    BookmarkManagerDialog *m_parent;

    BookmarkManager *const m_manager;

    GeoDataTreeModel *const m_treeModel;

    QSortFilterProxyModel m_folderFilterModel;

    QPersistentModelIndex m_selectedFolder;

    BranchFilterProxyModel m_branchFilterModel;

    BookmarkManagerDialogPrivate( BookmarkManagerDialog* parent, MarbleModel *model );

    void initializeFoldersView( GeoDataTreeModel* treeModel );

    void initializeBookmarksView( GeoDataTreeModel* treeModel );

    void handleFolderSelection( const QModelIndex &index );

    void updateButtonState();

    void addNewFolder();

    void renameFolder();

    void deleteFolder();

    void editBookmark();

    void deleteBookmark();

    void discardChanges();

    QModelIndex bookmarkTreeIndex( const QModelIndex &bookmark ) const;

    QModelIndex folderTreeIndex( const QModelIndex &index ) const;
    GeoDataContainer* selectedFolder();

    void selectFolder( const QString &name = QString(), const QModelIndex &index = QModelIndex() );
};

BookmarkManagerDialogPrivate::BookmarkManagerDialogPrivate( BookmarkManagerDialog* parent, MarbleModel *model ) :
    m_parent( parent ),
    m_manager( model->bookmarkManager() ),
    m_treeModel( model->treeModel() ),
    m_folderFilterModel(),
    m_branchFilterModel()
{
    // nothing to do
}

/// react to clicking on the folder index (of folderfiltermodel fame)
/// consequence is selecting this folder, or unselecting it and going to root folder
void BookmarkManagerDialogPrivate::handleFolderSelection( const QModelIndex &index )
{
    if( !index.isValid() ) {
        return;
    }
    Q_ASSERT( index.isValid() );
    Q_ASSERT( index.model() == &m_folderFilterModel );
    if( m_selectedFolder.isValid() &&
        m_parent->foldersTreeView->selectionModel()->selectedIndexes().contains( m_selectedFolder ) ) {
        m_selectedFolder = QModelIndex();
        m_parent->foldersTreeView->selectionModel()->clear();
        selectFolder();
    } else {
        m_selectedFolder = index;
        m_branchFilterModel.setBranchIndex( m_treeModel, folderTreeIndex( index ) );
        m_parent->bookmarksListView->setRootIndex(
                    m_branchFilterModel.mapFromSource( folderTreeIndex( index ) ) );
        m_parent->bookmarksListView->selectionModel()->clear();
    }
}

void BookmarkManagerDialogPrivate::updateButtonState()
{
    bool const hasFolderSelection = !m_parent->foldersTreeView->selectionModel()->selectedIndexes().isEmpty();
    m_parent->renameFolderButton->setEnabled( hasFolderSelection );
    m_parent->removeFolderButton->setEnabled( hasFolderSelection );

    bool const hasBookmarkSelection = !m_parent->bookmarksListView->selectionModel()->selectedIndexes().isEmpty();
    m_parent->editBookmarkButton->setEnabled( hasBookmarkSelection );
    m_parent->removeBookmarkButton->setEnabled( hasBookmarkSelection );
}

void BookmarkManagerDialogPrivate::addNewFolder()
{
    QPointer<NewBookmarkFolderDialog> dialog = new NewBookmarkFolderDialog( m_parent );
    if ( dialog->exec() == QDialog::Accepted && !dialog->folderName().isEmpty() ) {
        m_manager->addNewBookmarkFolder( selectedFolder(), dialog->folderName() );
        selectFolder( dialog->folderName(), m_selectedFolder );
    }
    delete dialog;
}

void BookmarkManagerDialogPrivate::renameFolder()
{
    GeoDataFolder *folder = dynamic_cast<GeoDataFolder*>(selectedFolder());
    if ( folder ) {
        QPointer<NewBookmarkFolderDialog> dialog = new NewBookmarkFolderDialog( m_parent );
        dialog->setFolderName( folder->name() );
        QPersistentModelIndex parentIndex = m_selectedFolder.parent();
        if ( dialog->exec() == QDialog::Accepted ) {
            m_manager->renameBookmarkFolder( folder, dialog->folderName() );
        }
        selectFolder( dialog->folderName(), parentIndex );
        delete dialog;
    }
}

void BookmarkManagerDialogPrivate::deleteFolder()
{
    GeoDataFolder *folder = dynamic_cast<GeoDataFolder*>(selectedFolder());
    if ( folder ) {
        if ( folder->size() > 0 ) {
            QString const text = QObject::tr( "The folder %1 is not empty. Removing it will delete all bookmarks it contains. Are you sure you want to delete the folder?" ).arg( folder->name() );
            if (QMessageBox::question(m_parent, BookmarkManagerDialog::tr("Remove Folder"), text, QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes) {
                return;
            }
        }

        // take note of the parentIndex before removing the folder
        QString parent = static_cast<GeoDataContainer*>(folder->parent())->name();
        QPersistentModelIndex greatParentIndex = m_selectedFolder.parent().parent();
        m_manager->removeBookmarkFolder( folder );
        selectFolder( parent, greatParentIndex);
    }
}

void BookmarkManagerDialogPrivate::editBookmark()
{
    QModelIndexList selection = m_parent->bookmarksListView->selectionModel()->selectedIndexes();
    if ( selection.size() == 1 ) {
        QModelIndex index = m_branchFilterModel.mapToSource( selection.first() );
        Q_ASSERT( index.isValid() );
        GeoDataObject* object = qvariant_cast<GeoDataObject*>( index.data( MarblePlacemarkModel::ObjectPointerRole ) );
        Q_ASSERT( object );
        GeoDataPlacemark* bookmark = dynamic_cast<GeoDataPlacemark*>( object );
        // do not try to edit folders
        if ( !bookmark ) {
            return;
        }
        Q_ASSERT( bookmark );
        QModelIndex treeIndex = index;
        Q_ASSERT( treeIndex.isValid() );
        QModelIndex folderIndex = treeIndex.parent();
        Q_ASSERT( folderIndex.isValid() );
        GeoDataObject* folderObject = qvariant_cast<GeoDataObject*>( folderIndex.data( MarblePlacemarkModel::ObjectPointerRole ) );
        Q_ASSERT( folderObject );
        GeoDataFolder* folder = dynamic_cast<GeoDataFolder*>( folderObject );
        Q_ASSERT( folder );

        QPointer<EditBookmarkDialog> dialog = new EditBookmarkDialog( m_manager, m_parent );
        dialog->setName( bookmark->name() );
        if ( bookmark->lookAt() ) {
            dialog->setRange( bookmark->lookAt()->range() );
        }
        dialog->setCoordinates( bookmark->coordinate() );
        dialog->setDescription( bookmark->description() );
        dialog->setFolderName( folder->name() );
        dialog->setIconLink( bookmark->style()->iconStyle().iconPath() );
        if ( dialog->exec() == QDialog::Accepted ) {
            bookmark->setName( dialog->name() );
            bookmark->setDescription( dialog->description() );
            bookmark->setCoordinate( dialog->coordinates() );
            GeoDataStyle::Ptr newStyle(new GeoDataStyle( *bookmark->style() ));
            newStyle->iconStyle().setIconPath( dialog->iconLink() );
            bookmark->setStyle( newStyle );
            if ( bookmark->lookAt() ) {
                bookmark->lookAt()->setCoordinates( dialog->coordinates() );
                bookmark->lookAt()->setRange( dialog->range() );
            } else if ( dialog->range() ) {
                GeoDataLookAt *lookat = new GeoDataLookAt;
                lookat->setCoordinates( dialog->coordinates() );
                lookat->setRange( dialog->range() );
                bookmark->setAbstractView( lookat );
            }
            m_manager->updateBookmark( bookmark );

            if (folder->name() != dialog->folder()->name() ) {
                GeoDataPlacemark newBookmark( *bookmark );
                m_manager->removeBookmark( bookmark );
                m_manager->addBookmark( dialog->folder(), newBookmark );
            }
        }
        delete dialog;
    }
}

void BookmarkManagerDialogPrivate::deleteBookmark()
{
    QModelIndexList selection = m_parent->bookmarksListView->selectionModel()->selectedIndexes();
    if ( selection.size() == 1 ) {
        QModelIndex bookmarkIndex = m_branchFilterModel.mapToSource( selection.first() );
        GeoDataFolder* folder = dynamic_cast<GeoDataFolder*>( selectedFolder() );
        if ( folder ) {
            GeoDataPlacemark* bookmark = dynamic_cast<GeoDataPlacemark*>( folder->child( bookmarkIndex.row() ) );
            if ( bookmark ) {
                m_manager->removeBookmark( bookmark );
            }
        }
    }
}

void BookmarkManagerDialogPrivate::discardChanges()
{
    m_manager->loadFile( "bookmarks/bookmarks.kml" );
}

/// selects the folder name from its parent (of folder filter fame)
void BookmarkManagerDialogPrivate::selectFolder( const QString &name, const QModelIndex &parent )
{
    if ( parent.isValid() ) {
        Q_ASSERT( parent.model() == &m_folderFilterModel );
    }

    if ( name.isEmpty() ) {
        QModelIndex documentTreeIndex = m_treeModel->index( m_parent->bookmarkDocument() );
        QModelIndex folderFilterIndex = m_folderFilterModel.mapFromSource( documentTreeIndex );
        Q_ASSERT( folderFilterIndex.isValid() );
        m_parent->foldersTreeView->setCurrentIndex( folderFilterIndex );
        handleFolderSelection( folderFilterIndex );
        return;
    }

    for ( int i=0; i < m_folderFilterModel.rowCount( parent ); ++i ) {
        QModelIndex childIndex = m_folderFilterModel.index( i, 0, parent );
        if ( childIndex.data().toString() == name
            && m_selectedFolder != childIndex ) {
            m_parent->foldersTreeView->setCurrentIndex( childIndex );
            handleFolderSelection( childIndex );
            return;
        }
        if ( m_folderFilterModel.hasChildren( childIndex ) ) {
            selectFolder( name, childIndex );
        }
    }
}

QModelIndex BookmarkManagerDialogPrivate::folderTreeIndex( const QModelIndex &index ) const
{
    Q_ASSERT( index.isValid() );
    Q_ASSERT( index.model() == &m_folderFilterModel );
    QModelIndex const treeModelIndex = m_folderFilterModel.mapToSource( index );
    Q_ASSERT( treeModelIndex.isValid() );
    Q_ASSERT( treeModelIndex.model() == m_treeModel );
    return treeModelIndex;
}

GeoDataContainer *BookmarkManagerDialogPrivate::selectedFolder()
{
    if( m_selectedFolder.isValid() ) {
        GeoDataObject* object = qvariant_cast<GeoDataObject*>( m_selectedFolder.data( MarblePlacemarkModel::ObjectPointerRole ) );
        Q_ASSERT( object );
        GeoDataContainer* container = dynamic_cast<GeoDataContainer*>( object );
        Q_ASSERT( container );
        return container;
    } else {
        return m_parent->bookmarkDocument();
    }
}

void BookmarkManagerDialogPrivate::initializeFoldersView( GeoDataTreeModel* treeModel )
{
    m_folderFilterModel.setFilterKeyColumn( 1 );
    const QString regexp = QLatin1String(GeoDataTypes::GeoDataFolderType) + QLatin1Char('|') + QLatin1String(GeoDataTypes::GeoDataDocumentType);
    m_folderFilterModel.setFilterRegExp( regexp );
    m_folderFilterModel.setSourceModel( treeModel );

    m_parent->foldersTreeView->setModel( &m_folderFilterModel );
    m_parent->foldersTreeView->setEditTriggers( QAbstractItemView::NoEditTriggers );
    m_parent->foldersTreeView->setHeaderHidden( true );
    for ( int i=1; i<m_treeModel->columnCount(); ++i ) {
        m_parent->foldersTreeView->hideColumn( i );
    }
    m_parent->foldersTreeView->setRootIndex( m_folderFilterModel.mapFromSource(
                                                 m_treeModel->index( m_parent->bookmarkDocument() )));

    m_parent->connect( m_parent->foldersTreeView,
            SIGNAL(clicked(QModelIndex)),
            m_parent, SLOT(handleFolderSelection(QModelIndex)) );
    m_parent->connect( m_parent->foldersTreeView->selectionModel(),
                      SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                      m_parent, SLOT(updateButtonState()) );
    m_parent->connect( m_parent->renameFolderButton, SIGNAL(clicked(bool)),
                       m_parent, SLOT(renameFolder()) );
    m_parent->connect( m_parent->newFolderButton, SIGNAL(clicked(bool)),
                       m_parent, SLOT(addNewFolder()) );
    m_parent->connect( m_parent->removeFolderButton, SIGNAL(clicked(bool)),
                       m_parent, SLOT(deleteFolder()) );
}

void BookmarkManagerDialogPrivate::initializeBookmarksView( GeoDataTreeModel* treeModel )
{
    m_branchFilterModel.setSourceModel( treeModel );

    m_parent->bookmarksListView->setModel( &m_branchFilterModel );
    m_parent->bookmarksListView->setEditTriggers( QAbstractItemView::NoEditTriggers );

    m_parent->connect( m_parent->bookmarksListView->selectionModel(),
                      SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                      m_parent, SLOT(updateButtonState()) );
    m_parent->connect( m_parent->editBookmarkButton, SIGNAL(clicked(bool)),
                       m_parent, SLOT(editBookmark()) );
    m_parent->connect( m_parent->removeBookmarkButton, SIGNAL(clicked(bool)),
                       m_parent, SLOT(deleteBookmark()) );
}

BookmarkManagerDialog::BookmarkManagerDialog( MarbleModel* model, QWidget *parent )
    : QDialog( parent ),
      d( new BookmarkManagerDialogPrivate( this, model ) )
{
    setupUi( this );
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    importButton->setVisible( !smallScreen );
    exportButton->setVisible( !smallScreen );
    foldersLabel->setVisible( !smallScreen );
    bookmarkLabel->setVisible( !smallScreen );

    d->initializeFoldersView( d->m_treeModel );
    d->initializeBookmarksView( d->m_treeModel );
    d->updateButtonState();

    connect( this, SIGNAL(accepted()), SLOT(saveBookmarks()) );
    connect( this, SIGNAL(rejected()), SLOT(discardChanges()) );
    connect( exportButton, SIGNAL(clicked()), this, SLOT(exportBookmarks()) );
    connect( importButton, SIGNAL(clicked()), this, SLOT(importBookmarks()) );

    d->selectFolder();
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
        if (!GeoDataDocumentWriter::write(fileName, *bookmarkDocument())) {
            mDebug() << "Could not write the bookmarks file" << fileName;
            QString const text = tr( "Unable to save bookmarks. Please check that the file is writable." );
            QMessageBox::warning(this, tr("Bookmark Export"), text);
        }
    }
}

void BookmarkManagerDialog::importBookmarks()
{
    QString const file = QFileDialog::getOpenFileName(this, tr("Import Bookmarks"),
                            QDir::homePath(), tr( "KML Files (*.kml)" ) );
    if ( file.isEmpty() ) {
        return;
    }

    GeoDataDocument *import = BookmarkManager::openFile( file );
    if ( !import ) {
        QString const text = tr( "The file %1 cannot be opened as a KML file." ).arg( file );
        QMessageBox::warning(this, tr( "Bookmark Import"), text);
        return;
    }

    GeoDataDocument* current = bookmarkDocument();

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

                        QPointer<QMessageBox> messageBox = new QMessageBox( this );
                        QString const intro = tr( "The file contains a bookmark that already exists among your Bookmarks." );
                        QString const newBookmark = tr( "Imported bookmark" );
                        QString const existingBookmark = tr( "Existing bookmark" );
                        QString const question = tr( "Do you want to replace the existing bookmark with the imported one?" );
                        QString html = QLatin1String("<p>%1</p><table><tr><td>%2</td><td><b>%3 / %4</b></td></tr>"
                                                     "<tr><td>%5</td><td><b>%6 / %7</b></td></tr></table><p>%8</p>");
                        html = html.arg( intro ).arg( existingBookmark ).arg( existingFolder->name() );
                        html = html.arg( existingPlacemark->name() ).arg( newBookmark ).arg( newFolder->name() );
                        html = html.arg( newPlacemark->name() ).arg( question );
                        messageBox->setText( html );

                        QAbstractButton *replaceButton    = messageBox->addButton(tr( "Replace" ),     QMessageBox::ActionRole );
                        QAbstractButton *replaceAllButton = messageBox->addButton(tr( "Replace All" ), QMessageBox::ActionRole );
                        QAbstractButton *skipButton       = messageBox->addButton(tr( "Skip" ),        QMessageBox::ActionRole );
                        QAbstractButton *skipAllButton    = messageBox->addButton(tr( "Skip All" ),    QMessageBox::ActionRole );
                                                            messageBox->addButton(tr( "Cancel" ),      QMessageBox::RejectRole );
                        messageBox->setIcon( QMessageBox::Question );

                        if ( !replaceAll ) {
                            messageBox->exec();
                        }
                        if ( messageBox->clickedButton() == replaceAllButton ) {
                            replaceAll = true;
                        } else if ( messageBox->clickedButton() == skipAllButton ) {
                            skipAll = true;
                            added = true;
                        } else if ( messageBox->clickedButton() == skipButton ) {
                            added = true;
                            delete messageBox;
                            continue;
                        } else if ( messageBox->clickedButton() != replaceButton ) {
                            delete messageBox;
                            return;
                        }

                        if ( messageBox->clickedButton() == replaceButton || replaceAll ) {
                            d->m_manager->removeBookmark( existingPlacemark );
                            d->m_manager->addBookmark( newFolder, *newPlacemark );
                            mDebug() << "Placemark " << newPlacemark->name() << " replaces " << existingPlacemark->name();
                            added = true;
                            delete messageBox;
                            break;
                        }
                        delete messageBox;
                    }
                }
            }

            if ( !added ) {
                d->m_manager->addBookmark( newFolder, *newPlacemark );
            }
        }
    }

    d->selectFolder();
}

GeoDataDocument* BookmarkManagerDialog::bookmarkDocument()
{
    return d->m_manager->document();
}

void BookmarkManagerDialog::setButtonBoxVisible( bool visible )
{
    buttonBox->setVisible( visible );
    if ( !visible ) {
        disconnect( this, SIGNAL(rejected()), this, SLOT(discardChanges()) );
        connect( this, SIGNAL(rejected()), SLOT(saveBookmarks()) );
    }
}

}

#include "moc_BookmarkManagerDialog.cpp"
