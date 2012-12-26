//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "FileViewWidget.h"

// Qt
#include <QtGui/QSortFilterProxyModel>

// Marble
#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTreeModel.h"
#include "FileManager.h"

using namespace Marble;
// Ui
#include "ui_FileViewWidget.h"

namespace Marble
{

class FileViewWidgetPrivate
{

 public Q_SLOTS:
    void saveFile();
    void closeFile();
    void enableFileViewActions();

 public:
    Ui::FileViewWidget  m_fileViewUi;
    FileManager *m_fileManager;
};

FileViewWidget::FileViewWidget( QWidget *parent, Qt::WindowFlags f )
    : QWidget( parent, f ),
      d( new FileViewWidgetPrivate )
{
    d->m_fileViewUi.setupUi( this );
}

FileViewWidget::~FileViewWidget()
{
    delete d;
}

void FileViewWidget::setTreeModel( GeoDataTreeModel *model )
{
    d->m_fileViewUi.m_treeView->setModel( model );
    d->m_fileViewUi.m_treeView->setSelectionModel( model->selectionModel() );
    d->m_fileViewUi.m_treeView->setSortingEnabled( true );
    d->m_fileViewUi.m_treeView->sortByColumn( 0, Qt::AscendingOrder );
    d->m_fileViewUi.m_treeView->resizeColumnToContents( 0 );
    d->m_fileViewUi.m_treeView->resizeColumnToContents( 1 );
    connect( model->selectionModel(),
             SIGNAL( selectionChanged( QItemSelection, QItemSelection ) ),
             this,
             SLOT( enableFileViewActions() ) );
    connect( d->m_fileViewUi.m_treeView, SIGNAL( activated( QModelIndex ) ),
             this, SLOT( mapCenterOnTreeViewModel( QModelIndex ) ) );
}

void FileViewWidget::setFileManager( FileManager *manager )
{
    d->m_fileManager = manager;
    connect( d->m_fileViewUi.m_saveButton,  SIGNAL( clicked() ) ,
             this,       SLOT( saveFile() ) );
    connect( d->m_fileViewUi.m_closeButton, SIGNAL( clicked() ) ,
             this,    SLOT( closeFile() ) );
}

void FileViewWidgetPrivate::saveFile()
{
    QModelIndex index = m_fileViewUi.m_treeView->selectionModel()->selectedRows().first();
    GeoDataObject *object = static_cast<GeoDataObject*>( index.internalPointer() );
    GeoDataDocument *document = dynamic_cast<GeoDataDocument*>(object);
    if ( document ) {
        m_fileManager->saveFile( document );
    }
}

void FileViewWidgetPrivate::closeFile()
{
    QModelIndex index = m_fileViewUi.m_treeView->selectionModel()->selectedRows().first();
    GeoDataObject *object = static_cast<GeoDataObject*>( index.internalPointer() );
    GeoDataDocument *document = dynamic_cast<GeoDataDocument*>(object);
    if ( document ) {
        m_fileManager->closeFile( document );
    }
}

void FileViewWidgetPrivate::enableFileViewActions()
{
    bool tmp = false;
    if ( !m_fileViewUi.m_treeView->selectionModel()->selectedRows().isEmpty() ) {
        QModelIndex index = m_fileViewUi.m_treeView->selectionModel()->selectedRows().first();
        GeoDataObject *object = static_cast<GeoDataObject*>( index.internalPointer() );
        GeoDataDocument *document = dynamic_cast<GeoDataDocument*>(object);
        if ( document ) {
            tmp = document->documentRole() == Marble::UserDocument;
        }
    }
    m_fileViewUi.m_saveButton->setEnabled( tmp );
    m_fileViewUi.m_closeButton->setEnabled( tmp );
}

void FileViewWidget::mapCenterOnTreeViewModel( const QModelIndex &index )
{
    if( !index.isValid() ) {
        return;
    }
    GeoDataObject *object
        = static_cast<GeoDataObject*>( index.internalPointer() );
    if ( dynamic_cast<GeoDataPlacemark*>(object) )
    {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(object);
        emit centerOn( *placemark, true );
    }
    else if ( dynamic_cast<GeoDataContainer*>(object) ) {
        GeoDataLatLonAltBox box = dynamic_cast<GeoDataContainer*>( object )->latLonAltBox();
        emit centerOn( box, true );
    }
}

}

#include "FileViewWidget.moc"
