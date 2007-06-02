//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin   <ingwa@kde.org>"
//


#include "MarbleNavigator.h"

#include <QtCore/QtAlgorithms>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtGui/QStringListModel>


MarbleNavigator::MarbleNavigator(QWidget *parent)
    : QWidget( parent )
{
    setupUi( this );
 
    m_minimumzoom = 950;

    setFocusPolicy( Qt::NoFocus );

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
}


void MarbleNavigator::changeZoom(int zoom)
{
    // No infinite loops here
    // if (zoomSlider->value() != zoom)
    zoomSlider->setValue( zoom );
    zoomSlider->setMinimum( m_minimumzoom );
}


void MarbleNavigator::resizeEvent ( QResizeEvent * )
{
//            m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
//                                           QSizePolicy::Fixed );
    if ( height() < 100 ) {
        if ( !zoomSlider->isHidden() ) {
            zoomSlider->hide();
            m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
                                           QSizePolicy::Expanding );
        }
    } else {
        if ( zoomSlider->isHidden() ) {
            zoomSlider->show();
            m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
                                           QSizePolicy::Fixed );
        }
    }

} 


#ifndef Q_OS_MACX
#include "MarbleNavigator.moc"
#endif
