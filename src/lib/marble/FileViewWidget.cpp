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
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QPointer>

// Marble
#include "GeoDataLatLonAltBox.h"
#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTreeModel.h"
#include "FileManager.h"
#include "MarblePlacemarkModel.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "TreeViewDecoratorModel.h"
#include "EditPlacemarkDialog.h"

using namespace Marble;
// Ui
#include "ui_FileViewWidget.h"

namespace Marble
{

class FileViewWidgetPrivate
{

 public:
    explicit FileViewWidgetPrivate( FileViewWidget *parent );
    void setTreeModel( GeoDataTreeModel *model );
    void setFileManager( FileManager *manager );


 public Q_SLOTS:
    void saveFile();
    void closeFile();
    void enableFileViewActions();
    void contextMenu(const QPoint &pt);
    void showPlacemarkDialog();

 public:
    FileViewWidget *q;
    Ui::FileViewWidget  m_fileViewUi;
    MarbleWidget *m_widget;
    TreeViewDecoratorModel m_treeSortProxy;
    FileManager *m_fileManager;

    QMenu *m_contextMenu;
    QAction *m_viewPropertiesAction;
};

FileViewWidgetPrivate::FileViewWidgetPrivate( FileViewWidget *parent )
    :q( parent ),
    m_widget( 0 ),
    m_fileManager( 0 )
{
    m_contextMenu = new QMenu(q);
    m_viewPropertiesAction = new QAction(q);
    m_viewPropertiesAction->setText(QObject::tr("View Properties"));
    m_contextMenu->addAction(m_viewPropertiesAction);
    QObject::connect(m_viewPropertiesAction, SIGNAL(triggered()),
                     q, SLOT(showPlacemarkDialog()));
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

    connect( this, SIGNAL(centerOn(GeoDataPlacemark,bool)),
             d->m_widget, SLOT(centerOn(GeoDataPlacemark,bool)) );
    connect( this, SIGNAL(centerOn(GeoDataLatLonBox,bool)),
             d->m_widget, SLOT(centerOn(GeoDataLatLonBox,bool)) );
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
    m_fileViewUi.m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    QObject::connect( m_fileViewUi.m_treeView,
             SIGNAL(expanded(QModelIndex)),
             &m_treeSortProxy, SLOT(trackExpandedState(QModelIndex)) );
    QObject::connect( m_fileViewUi.m_treeView,
             SIGNAL(collapsed(QModelIndex)),
             &m_treeSortProxy, SLOT(trackCollapsedState(QModelIndex)) );
    QObject::connect( m_fileViewUi.m_treeView->selectionModel(),
             SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
             q, SLOT(enableFileViewActions()) );
    QObject::connect( m_fileViewUi.m_treeView, SIGNAL(activated(QModelIndex)),
             q, SLOT(mapCenterOnTreeViewModel(QModelIndex)) );
    QObject::connect( m_fileViewUi.m_treeView, SIGNAL(customContextMenuRequested(QPoint)),
                      q, SLOT(contextMenu(QPoint)) );
}

void FileViewWidgetPrivate::setFileManager( FileManager *manager )
{
    m_fileManager = manager;
    QObject::connect( m_fileViewUi.m_saveButton,  SIGNAL(clicked()) ,
             q, SLOT(saveFile()) );
    QObject::connect( m_fileViewUi.m_closeButton, SIGNAL(clicked()) ,
             q, SLOT(closeFile()) );
}

void FileViewWidgetPrivate::saveFile()
{
    QModelIndex index = m_fileViewUi.m_treeView->selectionModel()->selectedRows().first();
    GeoDataObject *object
        = index.model()->data( index, MarblePlacemarkModel::ObjectPointerRole ).value<GeoDataObject*>();
    GeoDataDocument *document = dynamic_cast<GeoDataDocument*>(object);
    if ( document && !document->fileName().isEmpty() ) {
        m_fileManager->saveFile( QFileDialog::getSaveFileName( q, "Select filename for KML document" ), document );
    }
}

void FileViewWidgetPrivate::closeFile()
{
    QModelIndex index = m_fileViewUi.m_treeView->selectionModel()->selectedRows().first();
    GeoDataObject *object
        = index.model()->data( index, MarblePlacemarkModel::ObjectPointerRole ).value<GeoDataObject*>();
    GeoDataDocument *document = dynamic_cast<GeoDataDocument*>(object);
    if ( document ) {
        m_fileManager->closeFile( document );
    }
}

void FileViewWidgetPrivate::enableFileViewActions()
{
    bool isUserDocument = false;
    if ( !m_fileViewUi.m_treeView->selectionModel()->selectedRows().isEmpty() ) {
        QModelIndex index = m_fileViewUi.m_treeView->selectionModel()->selectedRows().first();
        GeoDataObject *object
            = index.model()->data( index, MarblePlacemarkModel::ObjectPointerRole ).value<GeoDataObject*>();
        GeoDataDocument *document = dynamic_cast<GeoDataDocument*>(object);
        if ( document ) {
            isUserDocument = document->documentRole() == Marble::UserDocument;
        }
    }
    m_fileViewUi.m_saveButton->setEnabled( isUserDocument );
    m_fileViewUi.m_closeButton->setEnabled( isUserDocument );
}

void FileViewWidgetPrivate::contextMenu(const QPoint &pt)
{
    const QModelIndex index = m_fileViewUi.m_treeView->indexAt(pt);
    const QAbstractItemModel *model = m_fileViewUi.m_treeView->model();
    if (index.isValid()) {
        GeoDataObject *obj = model->data(index, MarblePlacemarkModel::ObjectPointerRole).value<GeoDataObject*>();
        const GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>(obj);

        if (placemark) {
            m_contextMenu->popup(m_fileViewUi.m_treeView->mapToGlobal(pt));
        }
    }
}

void FileViewWidgetPrivate::showPlacemarkDialog()
{
    const QModelIndex index = m_fileViewUi.m_treeView->currentIndex();
    const QAbstractItemModel *model = m_fileViewUi.m_treeView->model();

    GeoDataObject *obj = model->data(index, MarblePlacemarkModel::ObjectPointerRole).value<GeoDataObject*>();
    GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>(obj);
    if (placemark) {
        QPointer<EditPlacemarkDialog> dialog = new EditPlacemarkDialog(placemark, nullptr, q);
        dialog->setReadOnly(true);
        dialog->exec();
        delete dialog;
    }
}

void FileViewWidget::mapCenterOnTreeViewModel( const QModelIndex &index )
{
    if( !index.isValid() ) {
        return;
    }
    GeoDataObject *object
        = index.model()->data( index, MarblePlacemarkModel::ObjectPointerRole ).value<GeoDataObject*>();
    if ( dynamic_cast<GeoDataPlacemark*>(object) )
    {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(object);
        d->m_widget->model()->placemarkSelectionModel()->select( index, QItemSelectionModel::ClearAndSelect );
        emit centerOn( *placemark, true );
    }
    else if ( dynamic_cast<GeoDataContainer*>(object) ) {
        GeoDataLatLonAltBox box = dynamic_cast<GeoDataContainer*>( object )->latLonAltBox();
        emit centerOn( box, true );
    }
}

}

#include "moc_FileViewWidget.cpp"
