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
#include "GeoDataPlacemark.h"
#include "GeoDataTreeModel.h"
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

    QSortFilterProxyModel  m_treeSortProxy;
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

void FileViewWidget::setFileViewModel( FileViewModel *model )
{
    //set up everything for the FileModel
    d->m_fileViewUi.m_fileView->setModel( model );
    delete d->m_fileViewUi.m_fileView->selectionModel();
    d->m_fileViewUi.m_fileView->setSelectionModel( model->selectionModel() );
    connect( d->m_fileViewUi.m_fileView->selectionModel(),
             SIGNAL( selectionChanged( QItemSelection, QItemSelection ) ),
             this,
             SLOT( enableFileViewActions() ) );
    connect( d->m_fileViewUi.m_saveButton,  SIGNAL( clicked() ) ,
             model,       SLOT( saveFile() ) );
    connect( d->m_fileViewUi.m_closeButton, SIGNAL( clicked() ) ,
             model,    SLOT( closeFile() ) );
}

void FileViewWidget::setTreeModel( GeoDataTreeModel *model )
{
    d->m_treeSortProxy.setSourceModel( model );
    d->m_treeSortProxy.setDynamicSortFilter( true );
    d->m_fileViewUi.m_treeView->setModel( &d->m_treeSortProxy );
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
        = static_cast<GeoDataObject*>( d->m_treeSortProxy.mapToSource(index).internalPointer() );
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
