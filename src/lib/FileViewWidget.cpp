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
#include "MarblePlacemarkModel.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"

using namespace Marble;
// Ui
#include "ui_FileViewWidget.h"

namespace Marble
{

class FileViewWidgetPrivate
{

 public:
    FileViewWidgetPrivate( FileViewWidget *parent );
    void setTreeModel( GeoDataTreeModel *model );
    void setFileManager( FileManager *manager );


 public Q_SLOTS:
    void saveFile();
    void closeFile();
    void enableFileViewActions();

 public:
    FileViewWidget *q;
    Ui::FileViewWidget  m_fileViewUi;
    MarbleWidget *m_widget;
    QSortFilterProxyModel m_treeSortProxy;
    FileManager *m_fileManager;
};

FileViewWidgetPrivate::FileViewWidgetPrivate( FileViewWidget *parent )
    :q( parent ) {
}

FileViewWidget::FileViewWidget( QWidget *parent, Qt::WindowFlags f )
    : QWidget( parent, f ),
      d( new FileViewWidgetPrivate( this ) )
{
    d->m_fileViewUi.setupUi( this );
    layout()->setMargin( 0 );
}

FileViewWidget::~FileViewWidget()
{
    delete d;
}

void FileViewWidget::setMarbleWidget( MarbleWidget *widget )
{
    d->m_widget = widget;
    d->setTreeModel( d->m_widget->model()->treeModel() );
    d->setFileManager( d->m_widget->model()->fileManager() );

    connect( this, SIGNAL( centerOn( const GeoDataPlacemark &, bool ) ),
             d->m_widget, SLOT( centerOn( const GeoDataPlacemark &, bool ) ) );
    connect( this, SIGNAL( centerOn( const GeoDataLatLonBox &, bool ) ),
             d->m_widget, SLOT( centerOn( const GeoDataLatLonBox &, bool ) ) );
}

void FileViewWidgetPrivate::setTreeModel( GeoDataTreeModel *model )
{
    m_treeSortProxy.setSourceModel( model );
    m_treeSortProxy.setDynamicSortFilter( true );
    m_fileViewUi.m_treeView->setModel( &m_treeSortProxy );
    m_fileViewUi.m_treeView->setSortingEnabled( true );
    m_fileViewUi.m_treeView->sortByColumn( 0, Qt::AscendingOrder );
    m_fileViewUi.m_treeView->resizeColumnToContents( 0 );
    m_fileViewUi.m_treeView->resizeColumnToContents( 1 );
    QObject::connect( m_fileViewUi.m_treeView->selectionModel(),
             SIGNAL( selectionChanged( QItemSelection, QItemSelection ) ),
             q, SLOT( enableFileViewActions() ) );
    QObject::connect( m_fileViewUi.m_treeView, SIGNAL( activated( QModelIndex ) ),
             q, SLOT( mapCenterOnTreeViewModel( QModelIndex ) ) );
}

void FileViewWidgetPrivate::setFileManager( FileManager *manager )
{
    m_fileManager = manager;
    QObject::connect( m_fileViewUi.m_saveButton,  SIGNAL( clicked() ) ,
             q, SLOT( saveFile() ) );
    QObject::connect( m_fileViewUi.m_closeButton, SIGNAL( clicked() ) ,
             q, SLOT( closeFile() ) );
}

void FileViewWidgetPrivate::saveFile()
{
    QModelIndex index = m_fileViewUi.m_treeView->selectionModel()->selectedRows().first();
    GeoDataObject *object
        = qVariantValue<GeoDataObject*>( index.model()->data( index, MarblePlacemarkModel::ObjectPointerRole ) );
    GeoDataDocument *document = dynamic_cast<GeoDataDocument*>(object);
    if ( document ) {
        m_fileManager->saveFile( document );
    }
}

void FileViewWidgetPrivate::closeFile()
{
    QModelIndex index = m_fileViewUi.m_treeView->selectionModel()->selectedRows().first();
    GeoDataObject *object
        = qVariantValue<GeoDataObject*>( index.model()->data( index, MarblePlacemarkModel::ObjectPointerRole ) );
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
        GeoDataObject *object
            = qVariantValue<GeoDataObject*>( index.model()->data( index, MarblePlacemarkModel::ObjectPointerRole ) );
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
        = qVariantValue<GeoDataObject*>( index.model()->data( index, MarblePlacemarkModel::ObjectPointerRole ) );
    if ( dynamic_cast<GeoDataPlacemark*>(object) )
    {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(object);
        d->m_widget->model()->placemarkSelectionModel()->select( d->m_treeSortProxy.mapToSource( index ), QItemSelectionModel::ClearAndSelect );
        emit centerOn( *placemark, true );
    }
    else if ( dynamic_cast<GeoDataContainer*>(object) ) {
        GeoDataLatLonAltBox box = dynamic_cast<GeoDataContainer*>( object )->latLonAltBox();
        emit centerOn( box, true );
    }
}

}

#include "FileViewWidget.moc"
