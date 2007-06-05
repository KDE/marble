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

#include <lib/MarbleWidget.h>
#include <lib/katlasdirs.h>
#include <lib/maptheme.h>


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


void MarbleControlBox::addMarbleWidget(MarbleWidget *widget)
{
    m_widget = widget;

    // Make us aware of all the PlaceMarks in the MarbleModel so that
    // we can search in them.
    setLocations( m_widget->placeMarkModel() );

    // Initialize the LegendBrowser

    legendBrowser->setCheckedLocations( m_widget->showPlaces() );
    legendBrowser->setCheckedBorders( m_widget->showBorders() );
    legendBrowser->setCheckedWaterBodies( m_widget->showLakes() && m_widget->showRivers() );

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
    connect( legendBrowser, SIGNAL( toggledBorders( bool ) ),
             m_widget, SLOT( setShowBorders( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledWaterBodies( bool ) ),
             m_widget, SLOT( setShowRivers( bool ) ) );
    connect( legendBrowser, SIGNAL( toggledWaterBodies( bool ) ),
             m_widget, SLOT( setShowLakes( bool ) ) );
}


void MarbleControlBox::changeZoom(int zoom)
{
    // No infinite loops here
    // if (zoomSlider->value() != zoom)
    zoomSlider->setValue( zoom );
    zoomSlider->setMinimum( m_minimumzoom );
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
