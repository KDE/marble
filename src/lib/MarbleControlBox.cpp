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

#include <QtCore/QtAlgorithms>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtGui/QStringListModel>
#include <QtGui/QTextFrame>

#include "ui_MarbleControlBox.h"

#include <global.h>

#include <MarbleWidget.h>
#include <MarbleDirs.h>
#include <maptheme.h>


class MarbleControlBoxPrivate
{
 public:

    MarbleWidget  *m_widget;
    QString        m_searchTerm;
    bool           m_searchTriggered;
    int            m_minimumzoom;

    Ui::MarbleControlBox  uiWidget;
    QWidget              *m_navigationWidget;
    QWidget              *m_legendWidget;
    QWidget              *m_mapViewWidget;
    QWidget              *m_currentLocationWidget;
    QWidget              *m_fileViewWidget;

    QStandardItemModel   *m_mapthememodel;
};


// ================================================================


MarbleControlBox::MarbleControlBox(QWidget *parent)
    : QWidget( parent ),
      d( new MarbleControlBoxPrivate )
{
    d->m_widget = 0;
    d->m_searchTerm = QString();
    d->m_searchTriggered = false;
    // FIXME: Get this from the widget?
    d->m_minimumzoom = 950;

    d->uiWidget.setupUi( this );
 
    setFocusPolicy( Qt::NoFocus );
//    setFocusProxy( d->uiWidget.searchLineEdit );

    d->uiWidget.NavigationTab->setBackgroundRole( QPalette::Window );
    d->uiWidget.LegendTab->setBackgroundRole( QPalette::Window );
    d->uiWidget.MapViewTab->setBackgroundRole( QPalette::Window );
    d->uiWidget.CurrentLocationTab->setBackgroundRole( QPalette::Window );

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
    
//  d->m_currentLocationWidget->hide(); // Current location tab is hidden
                                    //by default
 //   toolBox->removeItem( 3 ); 
    d->uiWidget.toolBox->setCurrentIndex(0);
    
    //default
    setCurrentLocationTabShown( false );
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

    connect( d->uiWidget.moveLeftButton,  SIGNAL( clicked() ),
             this,                        SIGNAL( moveLeft() ) ); 
    connect( d->uiWidget.moveRightButton, SIGNAL( clicked() ),
             this,                        SIGNAL( moveRight() ) ); 
    connect( d->uiWidget.moveUpButton,    SIGNAL( clicked() ),
             this,                        SIGNAL( moveUp() ) ); 
    connect( d->uiWidget.moveDownButton,  SIGNAL( clicked() ),
             this,                        SIGNAL (moveDown() ) ); 

    connect(d->uiWidget.searchLineEdit,   SIGNAL( textChanged( const QString& ) ),
            this,                         SLOT( searchLineChanged( const QString& ) ) );

    connect( d->uiWidget.locationListView, SIGNAL( centerOn( const QModelIndex& ) ),
             this,                         SIGNAL( centerOn( const QModelIndex& ) ) );

    QStringList          mapthemedirs  = MapTheme::findMapThemes( "maps/earth" );
    d->m_mapthememodel = MapTheme::mapThemeModel( mapthemedirs );
    d->uiWidget.katlasThemeSelectView->setModel( d->m_mapthememodel );

    connect( d->uiWidget.katlasThemeSelectView, SIGNAL( selectMapTheme( const QString& ) ),
             this,                              SIGNAL( selectMapTheme( const QString& ) ) );
}

void MarbleControlBox::setupGpsOption()
{
    d->uiWidget.m_gpsDrawBox -> setEnabled( true );
    d->uiWidget.m_gpsGoButton -> setEnabled( false );

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
    setLocations( d->m_widget->placeMarkModel() );
    
    //d->uiWidget.m_fileView->setModel( widget->gpxFileModel() );

    // Initialize the LegendBrowser
    d->uiWidget.legendBrowser->setCheckedLocations( d->m_widget->showPlaces() );
    d->uiWidget.legendBrowser->setCheckedCities( d->m_widget->showCities() );
    d->uiWidget.legendBrowser->setCheckedTerrain( d->m_widget->showTerrain() );
    d->uiWidget.legendBrowser->setCheckedBorders( d->m_widget->showBorders() );
    d->uiWidget.legendBrowser->setCheckedWaterBodies( d->m_widget->showLakes()
                                                      && d->m_widget->showRivers() );
    d->uiWidget.legendBrowser->setCheckedGrid( d->m_widget->showGrid() );
    d->uiWidget.legendBrowser->setCheckedIceLayer( d->m_widget->showIceLayer() );
    d->uiWidget.legendBrowser->setCheckedRelief( d->m_widget->showRelief() );
    d->uiWidget.legendBrowser->setCheckedWindRose( d->m_widget->showWindRose() );
    d->uiWidget.legendBrowser->setCheckedScaleBar( d->m_widget->showScaleBar() );

    // Connect necessary signals.
    connect( this, SIGNAL(goHome()),         d->m_widget, SLOT(goHome()) );
    connect( this, SIGNAL(zoomChanged(int)), d->m_widget, SLOT(zoomView(int)) );
    connect( this, SIGNAL(zoomIn()),         d->m_widget, SLOT(zoomIn()) );
    connect( this, SIGNAL(zoomOut()),        d->m_widget, SLOT(zoomOut()) );

    connect( this, SIGNAL(moveLeft()),  d->m_widget, SLOT(moveLeft()) );
    connect( this, SIGNAL(moveRight()), d->m_widget, SLOT(moveRight()) );
    connect( this, SIGNAL(moveUp()),    d->m_widget, SLOT(moveUp()) );
    connect( this, SIGNAL(moveDown()),  d->m_widget, SLOT(moveDown()) );

    connect( d->m_widget, SIGNAL(themeChanged( QString )), this, SLOT( selectTheme( QString )) );
    selectTheme( d->m_widget->mapTheme() );

    connect(d->m_widget, SIGNAL(zoomChanged(int)), 
	    this,     SLOT(changeZoom(int)));
    connect(this,     SIGNAL(centerOn(const QModelIndex&)),
	    d->m_widget, SLOT(centerOn(const QModelIndex&)));
    connect(this,     SIGNAL(selectMapTheme(const QString&)),
	    d->m_widget, SLOT(setMapTheme(const QString&)));

    // connect signals for the Legend
    connect( d->uiWidget.legendBrowser, SIGNAL( toggledLocations( bool ) ),
             d->m_widget,               SLOT( setShowPlaces( bool ) ) );
    connect( d->uiWidget.legendBrowser, SIGNAL( toggledCities( bool ) ),
             d->m_widget,               SLOT( setShowCities( bool ) ) );
    connect( d->uiWidget.legendBrowser, SIGNAL( toggledTerrain( bool ) ),
             d->m_widget,               SLOT( setShowTerrain( bool ) ) );
    connect( d->uiWidget.legendBrowser, SIGNAL( toggledBorders( bool ) ),
             d->m_widget,               SLOT( setShowBorders( bool ) ) );
    connect( d->uiWidget.legendBrowser, SIGNAL( toggledWaterBodies( bool ) ),
             d->m_widget,               SLOT( setShowRivers( bool ) ) );
    connect( d->uiWidget.legendBrowser, SIGNAL( toggledWaterBodies( bool ) ),
             d->m_widget,               SLOT( setShowLakes( bool ) ) );
    connect( d->uiWidget.legendBrowser, SIGNAL( toggledGrid( bool ) ),
             d->m_widget,               SLOT( setShowGrid( bool ) ) );
    connect( d->uiWidget.legendBrowser, SIGNAL( toggledIceLayer( bool ) ),
             d->m_widget,               SLOT( setShowIceLayer( bool ) ) );
    connect( d->uiWidget.legendBrowser, SIGNAL( toggledRelief( bool ) ),
             d->m_widget,               SLOT( setShowRelief( bool ) ) );
    connect( d->uiWidget.legendBrowser, SIGNAL( toggledWindRose( bool ) ),
             d->m_widget,               SLOT( setShowWindRose( bool ) ) );
    connect( d->uiWidget.legendBrowser, SIGNAL( toggledScaleBar( bool ) ),
             d->m_widget,               SLOT( setShowScaleBar( bool ) ) );
    
    //connect GPS Option signals
    connect( this, SIGNAL( gpsInputDisabled( bool ) ),
             d->m_widget, SLOT( setShowGps( bool ) ) );
    connect( this, SIGNAL( gpsPositionChanged( double, double ) ),
             d->m_widget, SLOT( changeCurrentPosition( double, double ) ) );
    connect( d->m_widget, SIGNAL( mouseClickGeoPosition( double, double, 
                                            GeoPoint::Unit ) ),
             this, SLOT( receiveGpsCoordinates ( double, double,
                                                 GeoPoint::Unit) ) );

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


void MarbleControlBox::setLocations(QAbstractItemModel* locations)
{
    d->uiWidget.locationListView->setModel( locations );
}

int MarbleControlBox::minimumZoom() const
{
    return d->m_minimumzoom;
}


void MarbleControlBox::changeZoom(int zoom)
{
    // No infinite loops here
    // if (zoomSlider->value() != zoom)
    d->uiWidget.zoomSlider->setValue( zoom );
    d->uiWidget.zoomSlider->setMinimum( d->m_minimumzoom );
}

void MarbleControlBox::disableGpsInput( bool in )
{
    d->uiWidget.m_latSpinBox->setEnabled( !in );
    d->uiWidget.m_lonSpinBox->setEnabled( !in );

    d->uiWidget.m_latComboBox->setEnabled( !in );
    d->uiWidget.m_lonComboBox->setEnabled( !in );

    double t_lat = d->uiWidget.m_latSpinBox->value();
    double t_lon = d->uiWidget.m_lonSpinBox->value();

    if( d->uiWidget.m_lonComboBox->currentIndex() == 1 ){
        t_lon *= -1;
    }
    
    if( d->uiWidget.m_latComboBox->currentIndex() == 1 ){
        t_lat *= -1;
    }
    
    emit gpsPositionChanged( t_lon, t_lat );
    emit gpsInputDisabled( in );
}

void MarbleControlBox::receiveGpsCoordinates( double x, double y, 
                                              GeoPoint::Unit unit){
    if( d->uiWidget.m_catchGps->isChecked() ){
        switch(unit){
        case GeoPoint::Degree:
            d->uiWidget.m_lonSpinBox->setValue( y );
            d->uiWidget.m_latSpinBox->setValue( x );
            emit gpsPositionChanged( y, x );
            break;
        case GeoPoint::Radian:
            double t_lat=0,t_lon=0;
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




void MarbleControlBox::resizeEvent ( QResizeEvent * )
{
    if ( height() < 480 ) {
        if ( !d->uiWidget.zoomSlider->isHidden() ) {
            d->uiWidget.zoomSlider->hide();
            d->uiWidget.m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
                                                       QSizePolicy::Expanding );
        }
    } else {
        if ( d->uiWidget.zoomSlider->isHidden() == true ) {
            d->uiWidget.zoomSlider->show();
            d->uiWidget.m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
                                                       QSizePolicy::Fixed );
        }
    }
} 

void MarbleControlBox::searchLineChanged(const QString &search)
{
    d->m_searchTerm = search;
    if ( d->m_searchTriggered )
        return;
    d->m_searchTriggered = true;
    QTimer::singleShot( 0, this, SLOT( search() ) );
}


void MarbleControlBox::search()
{
    d->m_searchTriggered = false;
    int  currentSelected = d->uiWidget.locationListView->currentIndex().row();
    d->uiWidget.locationListView->selectItem( d->m_searchTerm );
    if ( currentSelected != d->uiWidget.locationListView->currentIndex().row() )
        d->uiWidget.locationListView->activate();
}

void MarbleControlBox::selectTheme( QString theme )
{
    for ( int row = 0; row < d->m_mapthememodel->rowCount(); ++row ) {
        QModelIndex itIndexName = d->m_mapthememodel->index( row, 1, QModelIndex() );
        QModelIndex itIndex = d->m_mapthememodel->index( row, 0, QModelIndex() );
//        qDebug() << "Select Theme: " << theme << " Stored: " << d->m_mapthememodel->data( itIndexName ).toString();
        if ( theme == d->m_mapthememodel->data( itIndexName ).toString() ) {
              if ( itIndexName != d->uiWidget.katlasThemeSelectView->currentIndex() ) {
                d->uiWidget.katlasThemeSelectView->setCurrentIndex( itIndex );
                d->uiWidget.katlasThemeSelectView->scrollTo( itIndex );
                break;
              }
        }
    }
}

#include "MarbleControlBox.moc"
