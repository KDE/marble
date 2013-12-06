//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mihail Ivchenko <ematirov@gmail>
//

// Self
#include "TourWidget.h"

// Qt
#include <QFileDialog>
#include <QDir>

// Marble
#include "GeoDataDocument.h"
#include "GeoDataTreeModel.h"
#include "MarbleModel.h"
#include "ParsingRunnerManager.h"
#include "MarbleWidget.h"

using namespace Marble;
// Ui
#include "ui_TourWidget.h"

namespace Marble
{

class TourWidgetPrivate
{

 public:
    TourWidgetPrivate( TourWidget *parent );

 public Q_SLOTS:
    void openFile();

 public:
    TourWidget *q;
    Ui::TourWidget  m_tourUi;
    MarbleWidget *m_widget;
    GeoDataTreeModel *m_model;
};

TourWidgetPrivate::TourWidgetPrivate( TourWidget *parent )
    :q( parent )
{
    m_tourUi.setupUi( parent );
    m_model = new GeoDataTreeModel();
    m_tourUi.m_treeView->setModel( m_model );
    m_tourUi.m_treeView->setSortingEnabled( true );
    m_tourUi.m_treeView->sortByColumn( 0, Qt::AscendingOrder );
    m_tourUi.m_treeView->resizeColumnToContents( 0 );
    m_tourUi.m_treeView->resizeColumnToContents( 1 );
    QObject::connect( m_tourUi.m_openButton, SIGNAL( clicked() ), q, SLOT( openFile() ) );
}

TourWidget::TourWidget( QWidget *parent, Qt::WindowFlags f )
    : QWidget( parent, f ),
      d( new TourWidgetPrivate( this ) )
{
    layout()->setMargin( 0 );
}

TourWidget::~TourWidget()
{
    delete d;
}

void TourWidget::setMarbleWidget( MarbleWidget *widget )
{
    d->m_widget = widget;
}

void TourWidgetPrivate::openFile()
{
    QString filename = QFileDialog::getOpenFileName( q, q->tr( "Open Tour" ), QDir::homePath(), q->tr( "Tour (*.kml)" ) );
    ParsingRunnerManager manager( m_widget->model()->pluginManager() );
    GeoDataDocument* document = manager.openFile( filename );
    if ( document ) {
        m_model->addDocument( document );
    }
    m_widget->model()->addGeoDataFile( filename );
}

}

#include "TourWidget.moc"
