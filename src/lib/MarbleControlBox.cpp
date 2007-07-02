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


MarbleControlBox::MarbleControlBox(QWidget *parent)
    : QWidget( parent ),
      m_widget( 0 ),
      m_searchTriggered( false )
{
    setupUi( this );
 
    m_minimumzoom = 950;

    setFocusPolicy( Qt::NoFocus );

    toolBoxTab1->setBackgroundRole( QPalette::Window );
    toolBoxTab2->setBackgroundRole( QPalette::Window );

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
    
    QButtonGroup *latGroup = new QButtonGroup( this );
    QButtonGroup *lonGroup = new QButtonGroup( this );
    
    latGroup -> addButton( m_eRadio );
    latGroup -> addButton( m_wRadio );
    
    lonGroup -> addButton( m_nRadio );
    lonGroup -> addButton( m_sRadio );
    
    m_eRadio -> setChecked( true );
    m_nRadio -> setChecked( true );
    
    connect( m_gpsDrawBox, SIGNAL( clicked( bool ) ),
             this, SLOT( disableGpsInput( bool ) ) );
}


void MarbleControlBox::addMarbleWidget(MarbleWidget *widget)
{
    m_widget = widget;

    // Make us aware of all the PlaceMarks in the MarbleModel so that
    // we can search them.
    setLocations( m_widget->placeMarkModel() );

    // Initialize the LegendBrowser
    legendBrowser->setCheckedLocations( m_widget->showPlaces() );
    legendBrowser->setCheckedCities( m_widget->showCities() );
    legendBrowser->setCheckedTerrain( m_widget->showTerrain() );
    legendBrowser->setCheckedBorders( m_widget->showBorders() );
    legendBrowser->setCheckedWaterBodies( m_widget->showLakes()
                                          && m_widget->showRivers() );
    legendBrowser->setCheckedGrid( m_widget->showGrid() );
    legendBrowser->setCheckedIceLayer( m_widget->showIceLayer() );
    legendBrowser->setCheckedRelief( m_widget->showRelief() );
    legendBrowser->setCheckedWindRose( m_widget->showWindRose() );
    legendBrowser->setCheckedScaleBar( m_widget->showScaleBar() );

    // Connect necessary signals.
    connect( this, SIGNAL(goHome()),         m_widget, SLOT(goHome()) );
    connect( this, SIGNAL(zoomChanged(int)), m_widget, SLOT(zoomView(int)) );
    connect( this, SIGNAL(zoomIn()),         m_widget, SLOT(zoomIn()) );
    connect( this, SIGNAL(zoomOut()),        m_widget, SLOT(zoomOut()) );

    connect( this, SIGNAL(moveLeft()),  m_widget, SLOT(moveLeft()) );
    connect( this, SIGNAL(moveRight()), m_widget, SLOT(moveRight()) );
    connect( this, SIGNAL(moveUp()),    m_widget, SLOT(moveUp()) );
    connect( this, SIGNAL(moveDown()),  m_widget, SLOT(moveDown()) );

    connect(m_widget, SIGNAL(zoomChanged(int)), 
	    this,     SLOT(changeZoom(int)));
    connect(this,     SIGNAL(centerOn(const QModelIndex&)),
	    m_widget, SLOT(centerOn(const QModelIndex&)));
    connect(this,     SIGNAL(selectMapTheme(const QString&)),
	    m_widget, SLOT(setMapTheme(const QString&)));

    connect( legendBrowser, SIGNAL( toggledLocations( bool ) ),
             m_widget, SLOT( setShowPlaces( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledCities( bool ) ),
             m_widget, SLOT( setShowCities( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledTerrain( bool ) ),
             m_widget, SLOT( setShowTerrain( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledBorders( bool ) ),
             m_widget, SLOT( setShowBorders( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledWaterBodies( bool ) ),
             m_widget, SLOT( setShowRivers( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledWaterBodies( bool ) ),
             m_widget, SLOT( setShowLakes( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledGrid( bool ) ),
             m_widget, SLOT( setShowGrid( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledIceLayer( bool ) ),
             m_widget, SLOT( setShowIceLayer( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledRelief( bool ) ),
             m_widget, SLOT( setShowRelief( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledWindRose( bool ) ),
             m_widget, SLOT( setShowWindRose( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledScaleBar( bool ) ),
             m_widget, SLOT( setShowScaleBar( bool ) ) );
    
    //connect GPS Option signals
    connect( this, SIGNAL( gpsInputDisabled( bool ) ),
             m_widget, SLOT( setShowGps( bool ) ) );
    connect( this, SIGNAL( gpsPositionChanged( double, double ) ),
             m_widget, SLOT( changeGpsPosition( double, double ) ) );
    connect( m_widget, SIGNAL( gpsClickPos( double, double, 
                                            GeoPoint::Unit ) ),
             this, SLOT( recieveGpsCoordinates ( double, double,
                                                 GeoPoint::Unit) ) );

    connect( m_widget, SIGNAL( timeout() ), 
             this, SIGNAL( updateGps() ) );
}


void MarbleControlBox::setLocations(QAbstractItemModel* locations)
{
    locationListView->setModel( locations );
}

int MarbleControlBox::minimumZoom() const
{
    return m_minimumzoom;
}


void MarbleControlBox::changeZoom(int zoom)
{
    // No infinite loops here
    // if (zoomSlider->value() != zoom)
    zoomSlider->setValue( zoom );
    zoomSlider->setMinimum( m_minimumzoom );
}

void MarbleControlBox::disableGpsInput( bool in )
{
    m_latSpin->setEnabled( !in );
    m_lonSpin->setEnabled( !in );
    
    m_eRadio->setEnabled( !in );
    m_wRadio->setEnabled( !in );
    m_nRadio->setEnabled( !in );
    m_sRadio->setEnabled( !in );
    
    double t_lat = m_latSpin->value();
    double t_lon = m_lonSpin->value();
    
    if( m_wRadio->isChecked() ){
        t_lon *= -1;
    }
    
    if( m_sRadio->isChecked() ){
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
            m_latSpin->setValue( x );
            m_lonSpin->setValue( y );
            emit gpsPositionChanged( x, y );
            break;
        case GeoPoint::Radian:
            double rad2deg = 180.0/M_PI;
            double t_lat=0,t_lon=0;
            t_lat = y * -rad2deg;
            t_lon = x * rad2deg;
            
            if( t_lat < 0 ){
                m_latSpin->setValue( -t_lat );
                m_sRadio->setChecked( true );
            } else {
                m_latSpin->setValue( t_lat );
                m_nRadio->setChecked( true );
            }
            
            if( t_lon < 0 ){
                m_lonSpin->setValue( -t_lon );
                m_wRadio->setChecked( true );
            } else {
                m_lonSpin->setValue( t_lon );
                m_eRadio->setChecked( true );
            }
            
            emit gpsPositionChanged( t_lat, t_lon );
        }
    }
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
    m_searchTerm = search;
    if ( m_searchTriggered )
        return;
    m_searchTriggered = true;
    QTimer::singleShot( 0, this, SLOT( search() ) );
}


void MarbleControlBox::search()
{
    m_searchTriggered = false;
    int  currentSelected = locationListView->currentIndex().row();
    locationListView->selectItem( m_searchTerm );
    if ( currentSelected != locationListView->currentIndex().row() )
        locationListView->activate();
}


#include "MarbleControlBox.moc"
