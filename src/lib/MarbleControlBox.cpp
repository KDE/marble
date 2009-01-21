/* This file is part of the KDE project
 *
 * Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>"
 * Copyright 2007      Inge Wallin   <ingwa@kde.org>"
 * Copyright 2007      Thomas Zander <zander@kde.org>"
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


#include "MarbleControlBox.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtCore/QDataStream>
#include <QtCore/QtAlgorithms>
#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QStandardItemModel>
#include <QtGui/QTextFrame>
#include <QtNetwork/QHttp>

#include <global.h>

#include <MarbleWidget.h>
#include <MarbleMap.h>
#include <ViewParams.h>
#include <MarbleModel.h>
#include <MarbleDirs.h>
#include <FileViewModel.h>
#include "gps/GpxFileModel.h"
#include "PlaceMarkContainer.h"
#include "MarblePlacemarkModel.h"
#include "MarbleRunnerManager.h"
#include "MathHelper.h"
#include "MapThemeSortFilterProxyModel.h"

#include "GeoOnfParser.h"
#include "GeoDataDocument.h"

using namespace Marble;

#include "ui_MarbleControlBox.h"

namespace Marble
{

class MarbleControlBoxPrivate
{
 public:
    MarbleControlBoxPrivate();
    MarbleWidget  *m_widget;
    QString        m_searchTerm;
    bool           m_searchTriggered;

    Ui::MarbleControlBox  uiWidget;
    QWidget              *m_navigationWidget;
    QWidget              *m_legendWidget;
    QWidget              *m_mapViewWidget;
    QWidget              *m_currentLocationWidget;
    QWidget              *m_currentLocation2Widget;
    QWidget              *m_fileViewWidget;

    QStandardItemModel     *m_mapThemeModel;
    QSortFilterProxyModel  *m_sortproxy;
    MapThemeSortFilterProxyModel *m_mapSortProxy;
    
    MarbleRunnerManager  *m_runnerManager;
};

MarbleControlBoxPrivate::MarbleControlBoxPrivate()
{
}

// ================================================================


MarbleControlBox::MarbleControlBox(QWidget *parent)
    : QWidget( parent ),
      d( new MarbleControlBoxPrivate )
{
    d->m_widget = 0;
    d->m_searchTerm.clear();
    d->m_searchTriggered = false;

    d->uiWidget.setupUi( this );

    setFocusPolicy( Qt::NoFocus );
//    setFocusProxy( d->uiWidget.searchLineEdit );

    //  Iterate through all of the Side Widget values  //
    d->uiWidget.toolBox->setCurrentIndex( 0 );
    d->m_navigationWidget = d->uiWidget.toolBox->currentWidget();

    d->uiWidget.toolBox->setCurrentIndex( 1 );
    d->m_legendWidget = d->uiWidget.toolBox->currentWidget();

    d->uiWidget.toolBox->setCurrentIndex( 2 );
    d->m_mapViewWidget = d->uiWidget.toolBox->currentWidget();

    d->uiWidget.toolBox->setCurrentIndex( 3 );
    d->m_currentLocationWidget = d->uiWidget.toolBox->currentWidget();

    d->uiWidget.toolBox->setCurrentIndex( 4 );
    d->m_fileViewWidget = d->uiWidget.toolBox->currentWidget();

    d->uiWidget.toolBox->setCurrentIndex( 5 );
    d->m_currentLocation2Widget = d->uiWidget.toolBox->currentWidget();

    d->m_sortproxy = new QSortFilterProxyModel( d->uiWidget.locationListView );
    d->uiWidget.locationListView->setModel( d->m_sortproxy );

    d->m_mapSortProxy = new MapThemeSortFilterProxyModel( this );

//  d->m_currentLocationWidget->hide(); // Current location tab is hidden
                                    //by default
 //   toolBox->removeItem( 3 );
    d->uiWidget.toolBox->setCurrentIndex(0);

    //default
    setCurrentLocationTabShown( false );
    setCurrentLocation2TabShown( false );
    setFileViewTabShown( false );

    setupGpsOption();

    connect( d->uiWidget.goHomeButton,  SIGNAL( clicked() ),
             this,                      SIGNAL( goHome() ) );
    connect( d->uiWidget.zoomSlider,    SIGNAL( valueChanged( int ) ),
             this,                      SIGNAL( zoomChanged( int ) ) );
    connect( d->uiWidget.zoomInButton,  SIGNAL( clicked() ),
             this,                      SIGNAL( zoomIn() ) );
    connect( d->uiWidget.zoomOutButton, SIGNAL( clicked() ),
             this,                      SIGNAL( zoomOut() ) );

    connect( d->uiWidget.zoomSlider,  SIGNAL( valueChanged( int ) ),
             this,                      SLOT( updateButtons( int ) ) );

    connect( d->uiWidget.moveLeftButton,  SIGNAL( clicked() ),
             this,                        SIGNAL( moveLeft() ) );
    connect( d->uiWidget.moveRightButton, SIGNAL( clicked() ),
             this,                        SIGNAL( moveRight() ) );
    connect( d->uiWidget.moveUpButton,    SIGNAL( clicked() ),
             this,                        SIGNAL( moveUp() ) );
    connect( d->uiWidget.moveDownButton,  SIGNAL( clicked() ),
             this,                        SIGNAL (moveDown() ) );

    connect( d->uiWidget.locationListView, SIGNAL( centerOn( const QModelIndex& ) ),
             this,                         SLOT( mapCenterOnSignal( const QModelIndex& ) ) );

    d->m_mapThemeModel = 0;

    connect( d->uiWidget.marbleThemeSelectView, SIGNAL( selectMapTheme( const QString& ) ),
             this,                              SIGNAL( selectMapTheme( const QString& ) ) );
    connect( d->uiWidget.projectionComboBox,    SIGNAL( currentIndexChanged( int ) ),
             this,                              SLOT( projectionSelected( int ) ) );

    connect( d->uiWidget.zoomSlider,  SIGNAL( sliderPressed() ),
             this,                      SLOT( adjustForAnimation() ) );
    connect( d->uiWidget.zoomSlider,  SIGNAL( sliderReleased() ),
             this,                      SLOT( adjustForStill() ) );

    d->uiWidget.projectionComboBox->setEnabled( true );
    
    d->m_runnerManager = new MarbleRunnerManager( this );
    
    connect( d->m_runnerManager, SIGNAL( modelChanged(  MarblePlacemarkModel* ) ),
             this,               SLOT( setLocations( MarblePlacemarkModel* ) ) );    

    connect( d->uiWidget.searchLineEdit,  SIGNAL( textChanged( const QString& ) ),
             this,                        SLOT( searchLineChanged( const QString& ) ) );
    connect( d->uiWidget.searchLineEdit,  SIGNAL( returnPressed() ),
             this,                        SLOT( searchReturnPressed() ) );

}

MarbleControlBox::~MarbleControlBox()
{
    delete d;
}

void MarbleControlBox::setMapThemeModel( QStandardItemModel *mapThemeModel ) {
    d->m_mapThemeModel = mapThemeModel;
    d->m_mapSortProxy->setSourceModel( d->m_mapThemeModel );
    d->m_mapSortProxy->sort( 0 );
    d->uiWidget.marbleThemeSelectView->setModel( d->m_mapSortProxy );
    connect( d->m_mapThemeModel,       SIGNAL( rowsInserted( QModelIndex, int, int ) ),
             this,                     SLOT( updateMapThemeView() ) );
    updateMapThemeView();
}


void MarbleControlBox::updateButtons( int value )
{
    if ( value <= d->uiWidget.zoomSlider->minimum() ) {
        d->uiWidget.zoomInButton->setEnabled( true );
        d->uiWidget.zoomOutButton->setEnabled( false );
    } else if ( value >= d->uiWidget.zoomSlider->maximum() ) {
        d->uiWidget.zoomInButton->setEnabled( false );
        d->uiWidget.zoomOutButton->setEnabled( true );
    } else {
        d->uiWidget.zoomInButton->setEnabled( true );
        d->uiWidget.zoomOutButton->setEnabled( true );
    }
}


void MarbleControlBox::setupGpsOption()
{
    d->uiWidget.m_gpsDrawBox->setEnabled( true );
    d->uiWidget.m_gpsGoButton->setEnabled( false );

    d->uiWidget.m_latComboBox->setCurrentIndex( 0 );
    d->uiWidget.m_lonComboBox->setCurrentIndex( 0 );

    connect( d->uiWidget.m_gpsDrawBox, SIGNAL( clicked( bool ) ),
             this,                     SLOT( disableGpsInput( bool ) ) );
}


void MarbleControlBox::addMarbleWidget(MarbleWidget *widget)
{
    d->m_widget = widget;

    // Make us aware of all the PlaceMarks in the MarbleModel so that
    // we can search them.
    setLocations( static_cast<MarblePlacemarkModel*>(d->m_widget->placeMarkModel()) );
    d->uiWidget.locationListView->setSelectionModel( d->m_widget->placeMarkSelectionModel() );

#ifndef KML_GSOC
    //set up everything for the FileModel
    d->uiWidget.m_fileView->setModel( widget->fileViewModel() );

    connect( d->uiWidget.m_fileView->selectionModel(),
	     SIGNAL( selectionChanged( QItemSelection, QItemSelection )),
	     this,
	     SLOT( enableFileViewActions() ) );
    connect( d->uiWidget.m_saveButton,  SIGNAL( clicked() ) ,
             widget->fileViewModel(),    SLOT( saveFile() ) );
    connect( d->uiWidget.m_closeButton, SIGNAL( clicked() ) ,
             widget->fileViewModel(),    SLOT( closeFile() ) );
#else
    FileViewModel* model = widget->fileViewModel();
    d->uiWidget.m_fileView->setModel( model );

    connect( d->uiWidget.m_fileView->selectionModel(),
             SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& )),
             this,
             SLOT( enableFileViewActions() ) );

    connect( d->uiWidget.m_saveButton,  SIGNAL( clicked() ),
             model,                     SLOT( saveFile() ) );

    connect( d->uiWidget.m_closeButton, SIGNAL( clicked () ),
             model,                     SLOT( closeFile() ) );
#endif

    // Initialize the MarbleLegendBrowser
    d->uiWidget.marbleLegendBrowser->setMarbleWidget( d->m_widget );

    // Connect necessary signals.
    connect( this, SIGNAL(goHome()),         d->m_widget, SLOT(goHome()) );
    connect( this, SIGNAL(zoomChanged(int)), d->m_widget, SLOT(zoomView(int)) );
    connect( this, SIGNAL(zoomIn()),         d->m_widget, SLOT(zoomIn()) );
    connect( this, SIGNAL(zoomOut()),        d->m_widget, SLOT(zoomOut()) );

    connect( this, SIGNAL(moveLeft()),  d->m_widget, SLOT(moveLeft()) );
    connect( this, SIGNAL(moveRight()), d->m_widget, SLOT(moveRight()) );
    connect( this, SIGNAL(moveUp()),    d->m_widget, SLOT(moveUp()) );
    connect( this, SIGNAL(moveDown()),  d->m_widget, SLOT(moveDown()) );

    connect( this,        SIGNAL( projectionSelected( Projection ) ),  
             d->m_widget, SLOT( setProjection( Projection ) ) );

    connect( d->m_widget, SIGNAL( themeChanged( QString ) ),
             this,        SLOT( selectTheme( QString ) ) );

    connect( d->m_widget, SIGNAL( projectionChanged( Projection ) ),
             this,        SLOT( selectProjection( Projection ) ) );
    selectProjection( d->m_widget->projection() );

    connect( d->m_widget, SIGNAL( zoomChanged( int ) ),
	    this,        SLOT( changeZoom( int ) ) );
    connect( this,        SIGNAL( centerOn( const QModelIndex&, bool ) ),
	    d->m_widget, SLOT( centerOn( const QModelIndex&, bool ) ) );
    connect( this,        SIGNAL( selectMapTheme( const QString& ) ),
	    d->m_widget, SLOT( setMapThemeId( const QString& ) ) );

    // connect signals for the Legend

    connect( d->uiWidget.marbleLegendBrowser, SIGNAL( toggledShowProperty( QString, bool ) ),
             d->m_widget,                     SLOT( setPropertyValue( QString, bool ) ) );

    //connect CurrentLoctaion signals
    connect( this, SIGNAL( gpsInputDisabled( bool ) ),
             d->m_widget, SLOT( setShowGps( bool ) ) );
    connect( this, SIGNAL( gpsPositionChanged( qreal, qreal ) ),
             d->m_widget, SLOT( changeCurrentPosition( qreal, qreal ) ) );
    connect( d->m_widget, SIGNAL( mouseClickGeoPosition( qreal, qreal,
                                            GeoDataCoordinates::Unit ) ),
             this, SLOT( receiveGpsCoordinates ( qreal, qreal,
                                                 GeoDataCoordinates::Unit) ) );

    connect( d->m_widget, SIGNAL( timeout() ),
             this,        SIGNAL( updateGps() ) );
}

void MarbleControlBox::setWidgetTabShown( QWidget * widget,
                                          int insertIndex, bool show,
                                          QString &text )
{
    int index = d->uiWidget.toolBox->indexOf( widget );

    if( show ) {
        if ( !(index >= 0) ){
            if ( insertIndex < d->uiWidget.toolBox->count() ) {
                d->uiWidget.toolBox->insertItem( insertIndex,
                                                 widget,
                                                 text );
            } else {
                d->uiWidget.toolBox->insertItem( 3 ,widget, text );
            }
            widget->show();
        }
    } else {
        if ( index >= 0 ) {
            widget->hide();
            d->uiWidget.toolBox->removeItem( index );
        }
    }
}


void MarbleControlBox::setLocations(MarblePlacemarkModel* locations)
{
    QTime t;
    t.start();
    d->m_sortproxy->setSourceModel( locations );
    d->m_sortproxy->setSortLocaleAware( true );
    d->m_sortproxy->sort( 0 );
    qDebug("MarbleControlBox (sort): Time elapsed: %d ms", t.elapsed());
}

int MarbleControlBox::minimumZoom() const
{
    return d->m_widget->minimumZoom();
}

void MarbleControlBox::updateMapThemeView()
{
    if (d->m_widget)
        selectTheme( d->m_widget->mapThemeId() );
}

void MarbleControlBox::changeZoom(int zoom)
{
    // No infinite loops here
    // if (zoomSlider->value() != zoom)
    d->uiWidget.zoomSlider->setValue( zoom );
    d->uiWidget.zoomSlider->setMinimum( minimumZoom() );
}

void MarbleControlBox::disableGpsInput( bool in )
{
    d->uiWidget.m_latSpinBox->setEnabled( !in );
    d->uiWidget.m_lonSpinBox->setEnabled( !in );

    d->uiWidget.m_latComboBox->setEnabled( !in );
    d->uiWidget.m_lonComboBox->setEnabled( !in );

    qreal t_lat = d->uiWidget.m_latSpinBox->value();
    qreal t_lon = d->uiWidget.m_lonSpinBox->value();

    if( d->uiWidget.m_lonComboBox->currentIndex() == 1 ){
        t_lon *= -1;
    }

    if( d->uiWidget.m_latComboBox->currentIndex() == 1 ){
        t_lat *= -1;
    }

    emit gpsPositionChanged( t_lon, t_lat );
    emit gpsInputDisabled( in );
}

void MarbleControlBox::receiveGpsCoordinates( qreal x, qreal y,
                                              GeoDataCoordinates::Unit unit)
{
    if ( d->uiWidget.m_catchGps->isChecked() ) {
        switch(unit){
        case GeoDataCoordinates::Degree:
            d->uiWidget.m_lonSpinBox->setValue( y );
            d->uiWidget.m_latSpinBox->setValue( x );
            emit gpsPositionChanged( y, x );
            break;
        case GeoDataCoordinates::Radian:
            qreal t_lat=0,t_lon=0;
            t_lat = y * -RAD2DEG;
            t_lon = x * +RAD2DEG;

            if( t_lat < 0 ){
                d->uiWidget.m_latSpinBox->setValue( -t_lat );
                d->uiWidget.m_latComboBox->setCurrentIndex( 1 );
            } else {
                d->uiWidget.m_latSpinBox->setValue( t_lat );
                d->uiWidget.m_latComboBox->setCurrentIndex( 0 );
            }

            if( t_lon < 0 ){
                d->uiWidget.m_lonSpinBox->setValue( -t_lon );
                d->uiWidget.m_lonComboBox->setCurrentIndex( 1 );
            } else {
                d->uiWidget.m_lonSpinBox->setValue( t_lon );
                d->uiWidget.m_lonComboBox->setCurrentIndex( 0 );
            }

            emit gpsPositionChanged( t_lon, t_lat );
        }
    }
}

void MarbleControlBox::enableFileViewActions()
{
    bool tmp = d->uiWidget.m_fileView->selectionModel()
            ->selectedIndexes().count() ==1;
    d->uiWidget.m_saveButton->setEnabled( tmp );
    d->uiWidget.m_closeButton->setEnabled( tmp );

#ifndef KML_GSOC
    if ( tmp ) {
        QModelIndex tmpIndex =
            d->uiWidget.m_fileView->selectionModel()->currentIndex();
        d->m_widget->gpxFileModel()->setSelectedIndex( tmpIndex );
    }
#else
    if ( tmp ) {
        FileViewModel& model = ( FileViewModel& ) *d->uiWidget.m_fileView->model();
        model.setSelectedIndex( d->uiWidget.m_fileView->selectionModel()->currentIndex() );
    }
#endif
}

void MarbleControlBox::setNavigationTabShown( bool show )
{
    QString  title = tr( "Navigation" );
    setWidgetTabShown( d->m_navigationWidget, 0, show, title);
}

void MarbleControlBox::setLegendTabShown( bool show )
{
    QString  title = tr( "Legend" );
    setWidgetTabShown( d->m_legendWidget, 1, show, title );
}

void MarbleControlBox::setMapViewTabShown( bool show )
{
    QString  title = tr( "Map View" );
    setWidgetTabShown( d->m_mapViewWidget, 2, show, title );
}

void MarbleControlBox::setCurrentLocationTabShown( bool show )
{
    QString  title = tr( "Current Location" );
    setWidgetTabShown( d->m_currentLocationWidget, 3, show, title );
}

void MarbleControlBox::setFileViewTabShown( bool show )
{
    QString  title = tr( "File View" );
    setWidgetTabShown( d->m_fileViewWidget, 4, show, title );
}

void MarbleControlBox::setCurrentLocation2TabShown( bool show )
{
    QString  title = tr( "Current Location" );
    setWidgetTabShown( d->m_currentLocation2Widget, 5, show, title );
}



void MarbleControlBox::resizeEvent ( QResizeEvent * )
{
    if ( height() < 500 ) {
        if ( !d->uiWidget.zoomSlider->isHidden() ) {
            setUpdatesEnabled(false);
            d->uiWidget.zoomSlider->hide();
            d->uiWidget.m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
                                                       QSizePolicy::Expanding );
            setUpdatesEnabled(true);
        }
    } else {
        if ( d->uiWidget.zoomSlider->isHidden() == true ) {
            setUpdatesEnabled(false);
            d->uiWidget.zoomSlider->show();
            d->uiWidget.m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
                                                       QSizePolicy::Fixed );
            setUpdatesEnabled(true);
        }
    }
}

void MarbleControlBox::searchLineChanged(const QString &search)
{
    d->m_searchTerm = search;
    // if search line is empty, restore original geonames
    if ( d->m_searchTerm.isEmpty() )
        setLocations( static_cast<MarblePlacemarkModel*>( d->m_widget->placeMarkModel() ) );
    if ( d->m_searchTriggered )
        return;
    d->m_searchTriggered = true;
    QTimer::singleShot( 0, this, SLOT( search() ) );
}

void MarbleControlBox::searchReturnPressed()
{
    // do nothing if search term empty
    if ( d->m_searchTerm.isEmpty() ) {
        return;
    } else {
        d->m_runnerManager->newText( d->m_searchTerm );
        return;
    }
}


void MarbleControlBox::search()
{
    d->m_searchTriggered = false;
    int  currentSelected = d->uiWidget.locationListView->currentIndex().row();
    d->uiWidget.locationListView->selectItem( d->m_searchTerm );
    if ( currentSelected != d->uiWidget.locationListView->currentIndex().row() )
        d->uiWidget.locationListView->activate();
}

void MarbleControlBox::selectTheme( const QString &theme )
{
    if ( !d->m_mapSortProxy || !d->m_widget )
        return;

    // Check if the new selected theme is different from the current one
    QModelIndex currentIndex = d->uiWidget.marbleThemeSelectView->currentIndex();
    QString indexTheme = d->m_mapSortProxy->data( d->m_mapSortProxy->index( 
                         currentIndex.row(), 1, QModelIndex() ) ).toString();


    d->uiWidget.zoomSlider->setMaximum( d->m_widget->map()->maximumZoom() );
    updateButtons( d->uiWidget.zoomSlider->value() );
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
                QModelIndex iterIndex = items.at( 0 )->index();
                QModelIndex iterIndexName = d->m_mapSortProxy->mapFromSource( iterIndex.sibling( iterIndex.row(), 0 ) );

                d->uiWidget.marbleThemeSelectView->setCurrentIndex( iterIndexName );
                d->uiWidget.marbleThemeSelectView->scrollTo( iterIndexName );
            }
        }
    }
}

void MarbleControlBox::selectProjection( Projection projection )
{
    if ( (int)projection != d->uiWidget.projectionComboBox->currentIndex() )
        d->uiWidget.projectionComboBox->setCurrentIndex( (int) projection );
}

// Relay a signal and convert the parameter from an int to a Projection.
void MarbleControlBox::projectionSelected( int projectionIndex )
{
    emit projectionSelected( (Projection) projectionIndex );
}


void MarbleControlBox::mapCenterOnSignal( const QModelIndex &index )
{
    emit centerOn( d->m_sortproxy->mapToSource( index ), true );
}

void MarbleControlBox::adjustForAnimation()
{
    // TODO: use signals here as well 
    if ( !d->m_widget )
        return;

    d->m_widget->setViewContext( Marble::Animation );
}

void MarbleControlBox::adjustForStill()
{
    // TODO: use signals here as well
    if ( !d->m_widget )
        return;

    d->m_widget->setViewContext( Marble::Still );

    if ( d->m_widget->mapQuality( Marble::Still )
        != d->m_widget->mapQuality( Marble::Animation ) )
    {
        d->m_widget->updateChangedMap();
    }
}

}

#include "MarbleControlBox.moc"
