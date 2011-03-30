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
#include "GeoDataPlacemark.h"
#include "GeoDataTreeModel.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "FileViewModel.h"

using namespace Marble;
// Ui
#include "ui_FileViewWidget.h"

namespace Marble
{

class FileViewWidgetPrivate
{
 public:
    Ui::FileViewWidget  m_fileViewUi;
    MarbleWidget       *m_widget;

    QSortFilterProxyModel  *m_treeSortProxy;
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

void FileViewWidget::setMarbleWidget( MarbleWidget *widget )
{
    d->m_widget = widget;
    //set up everything for the FileModel
    d->m_fileViewUi.m_fileView->setModel( widget->model()->fileViewModel() );
    delete d->m_fileViewUi.m_fileView->selectionModel();
    d->m_fileViewUi.m_fileView->setSelectionModel( widget->model()->fileViewModel()->selectionModel() );
    connect( d->m_fileViewUi.m_fileView->selectionModel(),
             SIGNAL( selectionChanged( QItemSelection, QItemSelection ) ),
             this,
             SLOT( enableFileViewActions() ) );
    connect( d->m_fileViewUi.m_saveButton,  SIGNAL( clicked() ) ,
             widget->model()->fileViewModel(),       SLOT( saveFile() ) );
    connect( d->m_fileViewUi.m_closeButton, SIGNAL( clicked() ) ,
             widget->model()->fileViewModel(),    SLOT( closeFile() ) );

    d->m_treeSortProxy = new QSortFilterProxyModel( this );
    d->m_treeSortProxy->setSourceModel( widget->model()->treeModel() );
    d->m_treeSortProxy->setDynamicSortFilter( true );
    d->m_fileViewUi.m_treeView->setModel( d->m_treeSortProxy );
    d->m_fileViewUi.m_treeView->setSortingEnabled( true );
    d->m_fileViewUi.m_treeView->sortByColumn( 0, Qt::AscendingOrder );
    d->m_fileViewUi.m_treeView->resizeColumnToContents( 0 );
    d->m_fileViewUi.m_treeView->resizeColumnToContents( 1 );
    connect( d->m_fileViewUi.m_treeView, SIGNAL( activated( QModelIndex ) ),
             this, SLOT( mapCenterOnTreeViewModel( QModelIndex ) ) );
}

void FileViewWidget::enableFileViewActions()
{
    bool tmp = d->m_fileViewUi.m_fileView->selectionModel()->selectedIndexes().count() == 1;
    d->m_fileViewUi.m_saveButton->setEnabled( tmp );
    d->m_fileViewUi.m_closeButton->setEnabled( tmp );
}

void FileViewWidget::mapCenterOnTreeViewModel( const QModelIndex &index )
{
    if( !index.isValid() ) {
        return;
    }
    GeoDataObject *object
        = static_cast<GeoDataObject*>( d->m_treeSortProxy->mapToSource(index).internalPointer() );
    if ( dynamic_cast<GeoDataPlacemark*>(object) )
    {
        GeoDataCoordinates coord = ( dynamic_cast<GeoDataPlacemark*>( object ) )->coordinate();
        d->m_widget->centerOn( coord, true );
    }
}

}

#include "FileViewWidget.moc"
