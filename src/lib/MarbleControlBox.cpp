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

#include <MarbleWidget.h>
#include <katlasdirs.h>
#include <maptheme.h>


class MarbleControlBoxPrivate
{
 public:

    MarbleWidget  *m_widget;
    QString        m_searchTerm;
    bool           m_searchTriggered;
    int            m_minimumzoom;
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

    setupUi( this );
 
    setFocusPolicy( Qt::NoFocus );

    NavigationTab->setBackgroundRole( QPalette::Window );
    LegendTab->setBackgroundRole( QPalette::Window );
    MapViewTab->setBackgroundRole( QPalette::Window );
    CurrentLocationTab->setBackgroundRole( QPalette::Window );

    //  set all of the Side Widget Variables  //
    toolBox->setCurrentIndex( 0 );
    m_navigationWidget = toolBox->currentWidget();
    
    toolBox->setCurrentIndex( 1 );
    m_ledgendWidget = toolBox->currentWidget();
    
    toolBox->setCurrentIndex( 2 );
    m_mapViewWidget = toolBox->currentWidget();
    
    toolBox->setCurrentIndex( 3 );
    m_currentLocationWidget = toolBox->currentWidget();
    
//  m_currentLocationWidget->hide(); // Current location tab is hidden
                                    //by default
 //   toolBox->removeItem( 3 ); 
    toolBox->setCurrentIndex(0);
    
    //default
    setCurrentLocationTabShown( false );

    setupGpsOption();

    connect( goHomeButton, SIGNAL( clicked() ), 
             this,         SIGNAL( goHome() ) ); 
    connect( zoomSlider,   SIGNAL( valueChanged( int ) ),
             this,         SIGNAL( zoomChanged( int ) ) ); 
    connect( zoomInButton,  SIGNAL( clicked() ),
             this,          SIGNAL( zoomIn() ) ); 
    connect( zoomOutButton, SIGNAL( clicked() ),
             this,          SIGNAL( zoomOut() ) ); 

    connect( moveLeftButton,  SIGNAL( clicked() ),
             this,            SIGNAL( moveLeft() ) ); 
    connect( moveRightButton, SIGNAL( clicked() ),
             this,            SIGNAL( moveRight() ) ); 
    connect( moveUpButton,    SIGNAL( clicked() ),
             this,            SIGNAL( moveUp() ) ); 
    connect( moveDownButton,  SIGNAL( clicked() ),
             this,            SIGNAL (moveDown() ) ); 

    connect(searchLineEdit,   SIGNAL( textChanged( const QString& ) ),
            this,             SLOT( searchLineChanged( const QString& ) ) );

    connect( locationListView, SIGNAL( centerOn( const QModelIndex& ) ),
             this,             SIGNAL( centerOn( const QModelIndex& ) ) );

    QStringList          mapthemedirs  = MapTheme::findMapThemes( "maps/earth" );
    QStandardItemModel  *mapthememodel = MapTheme::mapThemeModel( mapthemedirs );
    katlasThemeSelectView->setModel( mapthememodel );

    connect( katlasThemeSelectView, SIGNAL( selectMapTheme( const QString& ) ),
             this,                  SIGNAL( selectMapTheme( const QString& ) ) );
}

void MarbleControlBox::setupGpsOption()
{
    m_gpsDrawBox -> setEnabled( true );
    m_gpsGoButton -> setEnabled( false );

    m_latComboBox->setCurrentIndex( 0 );
    m_lonComboBox->setCurrentIndex( 0 );
    
    connect( m_gpsDrawBox, SIGNAL( clicked( bool ) ),
             this, SLOT( disableGpsInput( bool ) ) );
}


void MarbleControlBox::addMarbleWidget(MarbleWidget *widget)
{
    d->m_widget = widget;

    // Make us aware of all the PlaceMarks in the MarbleModel so that
    // we can search them.
    setLocations( d->m_widget->placeMarkModel() );

    // Initialize the LegendBrowser
    legendBrowser->setCheckedLocations( d->m_widget->showPlaces() );
    legendBrowser->setCheckedCities( d->m_widget->showCities() );
    legendBrowser->setCheckedTerrain( d->m_widget->showTerrain() );
    legendBrowser->setCheckedBorders( d->m_widget->showBorders() );
    legendBrowser->setCheckedWaterBodies( d->m_widget->showLakes()
                                          && d->m_widget->showRivers() );
    legendBrowser->setCheckedGrid( d->m_widget->showGrid() );
    legendBrowser->setCheckedIceLayer( d->m_widget->showIceLayer() );
    legendBrowser->setCheckedRelief( d->m_widget->showRelief() );
    legendBrowser->setCheckedWindRose( d->m_widget->showWindRose() );
    legendBrowser->setCheckedScaleBar( d->m_widget->showScaleBar() );

    // Connect necessary signals.
    connect( this, SIGNAL(goHome()),         d->m_widget, SLOT(goHome()) );
    connect( this, SIGNAL(zoomChanged(int)), d->m_widget, SLOT(zoomView(int)) );
    connect( this, SIGNAL(zoomIn()),         d->m_widget, SLOT(zoomIn()) );
    connect( this, SIGNAL(zoomOut()),        d->m_widget, SLOT(zoomOut()) );

    connect( this, SIGNAL(moveLeft()),  d->m_widget, SLOT(moveLeft()) );
    connect( this, SIGNAL(moveRight()), d->m_widget, SLOT(moveRight()) );
    connect( this, SIGNAL(moveUp()),    d->m_widget, SLOT(moveUp()) );
    connect( this, SIGNAL(moveDown()),  d->m_widget, SLOT(moveDown()) );

    connect(d->m_widget, SIGNAL(zoomChanged(int)), 
	    this,     SLOT(changeZoom(int)));
    connect(this,     SIGNAL(centerOn(const QModelIndex&)),
	    d->m_widget, SLOT(centerOn(const QModelIndex&)));
    connect(this,     SIGNAL(selectMapTheme(const QString&)),
	    d->m_widget, SLOT(setMapTheme(const QString&)));

    connect( legendBrowser, SIGNAL( toggledLocations( bool ) ),
             d->m_widget, SLOT( setShowPlaces( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledCities( bool ) ),
             d->m_widget, SLOT( setShowCities( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledTerrain( bool ) ),
             d->m_widget, SLOT( setShowTerrain( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledBorders( bool ) ),
             d->m_widget, SLOT( setShowBorders( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledWaterBodies( bool ) ),
             d->m_widget, SLOT( setShowRivers( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledWaterBodies( bool ) ),
             d->m_widget, SLOT( setShowLakes( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledGrid( bool ) ),
             d->m_widget, SLOT( setShowGrid( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledIceLayer( bool ) ),
             d->m_widget, SLOT( setShowIceLayer( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledRelief( bool ) ),
             d->m_widget, SLOT( setShowRelief( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledWindRose( bool ) ),
             d->m_widget, SLOT( setShowWindRose( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledScaleBar( bool ) ),
             d->m_widget, SLOT( setShowScaleBar( bool ) ) );
    
    //connect GPS Option signals
    connect( this, SIGNAL( gpsInputDisabled( bool ) ),
             d->m_widget, SLOT( setShowGps( bool ) ) );
    connect( this, SIGNAL( gpsPositionChanged( double, double ) ),
             d->m_widget, SLOT( changeGpsPosition( double, double ) ) );
    connect( d->m_widget, SIGNAL( gpsClickPos( double, double, 
                                            GeoPoint::Unit ) ),
             this, SLOT( recieveGpsCoordinates ( double, double,
                                                 GeoPoint::Unit) ) );

    connect( d->m_widget, SIGNAL( timeout() ), 
             this,        SIGNAL( updateGps() ) );
}

void MarbleControlBox::setWidgetTabShown( QWidget * widget,
                                          int insertIndex, bool show,
                                          QString text )
{
    int index = toolBox->indexOf( widget );
    qDebug() << text << index;

    if( show ) {
        if ( !(index >= 0) ){
            if ( insertIndex < toolBox->count() ) {
                toolBox->insertItem( insertIndex,
                                     widget, 
                                     text );
            } else { 
                qDebug() << "here";
                toolBox->insertItem( 3 ,widget, text );
            }
            widget->show();
        }
    } else {
        if ( index >= 0 ) {
            widget->hide();
            toolBox->removeItem( index );
        }
    }
}


void MarbleControlBox::setLocations(QAbstractItemModel* locations)
{
    locationListView->setModel( locations );
}

int MarbleControlBox::minimumZoom() const
{
    return d->m_minimumzoom;
}


void MarbleControlBox::changeZoom(int zoom)
{
    // No infinite loops here
    // if (zoomSlider->value() != zoom)
    zoomSlider->setValue( zoom );
    zoomSlider->setMinimum( d->m_minimumzoom );
}

void MarbleControlBox::disableGpsInput( bool in )
{
    m_latSpinBox->setEnabled( !in );
    m_lonSpinBox->setEnabled( !in );

    m_latComboBox->setEnabled( !in );
    m_lonComboBox->setEnabled( !in );

    double t_lat = m_latSpinBox->value();
    double t_lon = m_lonSpinBox->value();

    if( m_lonComboBox->currentIndex() == 1 ){
        t_lon *= -1;
    }
    
    if( m_latComboBox->currentIndex() == 1 ){
        t_lat *= -1;
    }
    
    emit gpsPositionChanged( t_lat, t_lon );
    emit gpsInputDisabled( in );
}

void MarbleControlBox::recieveGpsCoordinates( double x, double y, 
                                              GeoPoint::Unit unit){
    if( m_catchGps->isChecked() ){
        switch(unit){
        case GeoPoint::Degree:
            m_latSpinBox->setValue( x );
            m_lonSpinBox->setValue( y );
            emit gpsPositionChanged( x, y );
            break;
        case GeoPoint::Radian:
            double rad2deg = 180.0/M_PI;
            double t_lat=0,t_lon=0;
            t_lat = y * -rad2deg;
            t_lon = x * rad2deg;
            
            if( t_lat < 0 ){
                m_latSpinBox->setValue( -t_lat );
                m_latComboBox->setCurrentIndex( 1 );
            } else {
                m_latSpinBox->setValue( t_lat );
                m_latComboBox->setCurrentIndex( 0 );
            }
            
            if( t_lon < 0 ){
                m_lonSpinBox->setValue( -t_lon );
                m_lonComboBox->setCurrentIndex( 1 );
            } else {
                m_lonSpinBox->setValue( t_lon );
                m_lonComboBox->setCurrentIndex( 0 );
            }
            
            emit gpsPositionChanged( t_lat, t_lon );
        }
    }
}

void MarbleControlBox::setNavigationTabShown( bool show )
{
    setWidgetTabShown( m_navigationWidget, 0, show, "Navigation" );
}

void MarbleControlBox::setLegendTabShown( bool show )
{
    setWidgetTabShown( m_ledgendWidget, 1, show, "Ledgend" );
}

void MarbleControlBox::setMapViewTabShown( bool show )
{
    setWidgetTabShown( m_mapViewWidget, 2, show, "Map View" );
}

void MarbleControlBox::setCurrentLocationTabShown( bool show )
{
    setWidgetTabShown( m_currentLocationWidget, 3, show, 
                       "Current Location" );
}


void MarbleControlBox::resizeEvent ( QResizeEvent * )
{
    if ( height() < 480 ) {
        if ( !zoomSlider->isHidden() ) {
            zoomSlider->hide();
            m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
                                           QSizePolicy::Expanding );
        }
    } else {
        if ( zoomSlider->isHidden() == true ) {
            zoomSlider->show();
            m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
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
    int  currentSelected = locationListView->currentIndex().row();
    locationListView->selectItem( d->m_searchTerm );
    if ( currentSelected != locationListView->currentIndex().row() )
        locationListView->activate();
}


#include "MarbleControlBox.moc"
