//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
// Copyright 2007      Thomas Zander <zander@kde.org>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "MapViewWidget.h"

// Marble
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "MapThemeManager.h"
#include "MapThemeSortFilterProxyModel.h"
#include "Planet.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"

// Qt
#include <QtGui/QStandardItemModel>
#include <QtGui/QGridLayout>

using namespace Marble;
// Ui
#include "ui_MapViewWidget.h"

namespace Marble
{

class MapViewWidgetPrivate {
 public:
    MapViewWidgetPrivate( MapViewWidget *parent )
        : m_parent( parent ),
          m_celestialList( 0 )
    {
    }

    void setMapThemeModel( QStandardItemModel *mapThemeModel )
    {
        m_mapThemeModel = mapThemeModel;
        m_mapSortProxy->setSourceModel( m_mapThemeModel );
        int currentIndex = m_mapViewUi.celestialBodyComboBox->currentIndex();
        QStandardItem * selectedItem = m_celestialList->item( currentIndex, 1 );

        if ( selectedItem ) {
            QString selectedId;
            selectedId = selectedItem->text();
            m_mapSortProxy->setFilterRegExp( QRegExp( selectedId, Qt::CaseInsensitive,QRegExp::FixedString ) );
        }

        m_mapSortProxy->sort( 0 );
        m_mapViewUi.marbleThemeSelectView->setModel( m_mapSortProxy );
        QObject::connect( m_mapThemeModel,       SIGNAL( rowsInserted( QModelIndex, int, int ) ),
                          m_parent,              SLOT( updateMapThemeView() ) );
    }

    MapViewWidget *m_parent;

    Ui::MapViewWidget  m_mapViewUi;
    MarbleWidget      *m_widget;

    QStandardItemModel     *m_mapThemeModel;
    MapThemeSortFilterProxyModel *m_mapSortProxy;

    QStandardItemModel *m_celestialList;
};

MapViewWidget::MapViewWidget( QWidget *parent, Qt::WindowFlags f )
    : QWidget( parent, f ),
      d( new MapViewWidgetPrivate( this ) )
{
    d->m_mapViewUi.setupUi( this );

    if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        QGridLayout* layout = new QGridLayout;
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 0 ), 0, 0 );
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 0 ), 0, 1 );
        d->m_mapViewUi.line->setVisible( false );
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 1 ), 1, 0 );
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 1 ), 1, 1 );
        d->m_mapViewUi.verticalLayout->insertLayout( 0, layout );
        d->m_mapViewUi.mapThemeLabel->setVisible( false );
    }

    d->m_mapSortProxy = new MapThemeSortFilterProxyModel( this );
    d->m_mapThemeModel = 0;

    connect( d->m_mapViewUi.marbleThemeSelectView, SIGNAL( selectMapTheme( const QString& ) ),
             this,                                 SIGNAL( selectMapTheme( const QString& ) ) );
    connect( d->m_mapViewUi.projectionComboBox,    SIGNAL( activated( int ) ),
             this,                                 SLOT( projectionSelected( int ) ) );

    d->m_mapViewUi.projectionComboBox->setEnabled( true );

    // Setting up the celestial combobox
    d->m_celestialList = new QStandardItemModel();

    d->m_mapViewUi.celestialBodyComboBox->setModel( d->m_celestialList );
    connect( d->m_mapViewUi.celestialBodyComboBox, SIGNAL( activated( const QString& ) ),
             this,                                 SLOT( selectCurrentMapTheme( const QString& ) ) );
    
    connect( d->m_mapViewUi.marbleThemeSelectView, SIGNAL( showMapWizard() ), this, SIGNAL( showMapWizard() ) );
    connect( d->m_mapViewUi.marbleThemeSelectView, SIGNAL( showUploadDialog() ), this, SIGNAL( showUploadDialog() ) );
}

MapViewWidget::~MapViewWidget()
{
    delete d->m_celestialList;
    delete d;
}

void MapViewWidget::updateCelestialModel()
{
    int row = d->m_mapThemeModel->rowCount();

    for ( int i = 0; i < row; ++i )
    {
        QString celestialBodyId = ( d->m_mapThemeModel->data( d->m_mapThemeModel->index( i, 1 ) ).toString() ).section( '/', 0, 0 );
        QString celestialBodyName = Planet::name( celestialBodyId );

        QList<QStandardItem*> matchingItems = d->m_celestialList->findItems ( celestialBodyId, Qt::MatchExactly, 1 );
        if ( matchingItems.isEmpty() ) {
            d->m_celestialList->appendRow( QList<QStandardItem*>()
                                << new QStandardItem( celestialBodyName )
                                << new QStandardItem( celestialBodyId ) );
        }
    }
}

void MapViewWidget::setMarbleWidget( MarbleWidget *widget )
{
    d->m_widget = widget;

    connect( this,        SIGNAL( projectionSelected( Projection ) ),
             d->m_widget, SLOT( setProjection( Projection ) ) );

    connect( d->m_widget, SIGNAL( themeChanged( QString ) ),
             this,        SLOT( selectTheme( QString ) ) );

    connect( d->m_widget, SIGNAL( projectionChanged( Projection ) ),
             this,        SLOT( selectProjection( Projection ) ) );
    selectProjection( d->m_widget->projection() );

    connect( this,        SIGNAL( selectMapTheme( const QString& ) ),
             d->m_widget, SLOT( setMapThemeId( const QString& ) ) );

    d->setMapThemeModel( widget->model()->mapThemeManager()->mapThemeModel() );
    updateMapThemeView();
}

void MapViewWidget::updateMapThemeView()
{
    updateCelestialModel();

    if ( d->m_widget ) {
        QString mapThemeId = d->m_widget->mapThemeId();
        if ( !mapThemeId.isEmpty() )
            selectTheme( mapThemeId );
    }
}

void MapViewWidget::selectTheme( const QString &theme )
{
    if ( !d->m_mapSortProxy || !d->m_widget )
        return;
    // Check if the new selected theme is different from the current one
    QModelIndex currentIndex = d->m_mapViewUi.marbleThemeSelectView->currentIndex();
    QString indexTheme = d->m_mapSortProxy->data( d->m_mapSortProxy->index(
                         currentIndex.row(), 1, QModelIndex() ) ).toString();

    if ( theme != indexTheme ) {
        /* indexTheme would be empty if the chosen map has not been set yet. As
        this needs to be done after the mapThemeId has been set, check if that is
        not empty first. The behaviour differs between Linux and Windows: on
        Windows the reading of the settings is not delayed, thus the mapThemeId
        is available earlier than on Linux.
        */
        if( indexTheme.isEmpty() && !d->m_widget->mapThemeId().isEmpty() ) {
            QList<QStandardItem*> items = d->m_mapThemeModel->findItems( theme, Qt::MatchExactly, 1 );
            if( items.size() >= 1 ) {
                QModelIndex iterIndex = items.first()->index();
                QModelIndex iterIndexName = d->m_mapSortProxy->mapFromSource( iterIndex.sibling( iterIndex.row(), 0 ) );

                d->m_mapViewUi.marbleThemeSelectView->setCurrentIndex( iterIndexName );

                d->m_mapViewUi.marbleThemeSelectView->scrollTo( iterIndexName );
            }
        }

        QString selectedId = d->m_widget->mapTheme()->head()->target();

        QList<QStandardItem*> itemList = d->m_celestialList->findItems( selectedId, Qt::MatchExactly, 1 );

        if ( !itemList.isEmpty() ) {
            QStandardItem * selectedItem = itemList.first();

            if ( selectedItem ) {
                int selectedIndex = selectedItem->row();
                d->m_mapViewUi.celestialBodyComboBox->setCurrentIndex( selectedIndex );
                d->m_mapSortProxy->setFilterRegExp( QRegExp( selectedId, Qt::CaseInsensitive,QRegExp::FixedString ) );
            }

            d->m_mapSortProxy->sort( 0 );
        }
    }
}

void MapViewWidget::selectProjection( Projection projection )
{
    if ( (int)projection != d->m_mapViewUi.projectionComboBox->currentIndex() )
        d->m_mapViewUi.projectionComboBox->setCurrentIndex( (int) projection );
}

void MapViewWidget::selectCurrentMapTheme( const QString& celestialBodyId )
{
    Q_UNUSED( celestialBodyId )

    d->setMapThemeModel( d->m_mapThemeModel );

    bool foundMapTheme = false;

    QString currentMapThemeId = d->m_widget->mapThemeId();

    int row = d->m_mapSortProxy->rowCount();

    for ( int i = 0; i < row; ++i )
    {
        QModelIndex index = d->m_mapSortProxy->index(i,1);
        QString itMapThemeId = d->m_mapSortProxy->data(index).toString();
        if ( currentMapThemeId == itMapThemeId )
        {
            foundMapTheme = true;
            break;
        }
    }
    if ( !foundMapTheme ) {
        QModelIndex index = d->m_mapSortProxy->index(0,1);
        d->m_widget->setMapThemeId( d->m_mapSortProxy->data(index).toString());
    }

    updateMapThemeView();
}

// Relay a signal and convert the parameter from an int to a Projection.
void MapViewWidget::projectionSelected( int projectionIndex )
{
    emit projectionSelected( (Projection) projectionIndex );
}

}

#include "MapViewWidget.moc"
