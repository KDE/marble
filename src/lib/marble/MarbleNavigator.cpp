//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
//


#include "MarbleNavigator.h"

#include <QtAlgorithms>

#include "MarbleDebug.h"

#include "ui_MarbleNavigator.h"

namespace Marble
{

class MarbleNavigatorPrivate
{
 public:
    int  m_minimumzoom;

    Ui::MarbleNavigator  uiWidget;
};


MarbleNavigator::MarbleNavigator( QWidget *parent )
    : QWidget( parent ),
      d( new MarbleNavigatorPrivate )

{
    d->uiWidget.setupUi( this );
 
    d->m_minimumzoom = 950;

    setFocusPolicy( Qt::NoFocus );

    connect( d->uiWidget.goHomeButton,  SIGNAL(clicked()), 
             this,                      SIGNAL(goHome()) ); 
    connect( d->uiWidget.zoomSlider,    SIGNAL(valueChanged(int)),
             this,                      SIGNAL(zoomChanged(int)) ); 
    connect( d->uiWidget.zoomInButton,  SIGNAL(clicked()),
             this,                      SIGNAL(zoomIn()) ); 
    connect( d->uiWidget.zoomOutButton, SIGNAL(clicked()),
             this,                      SIGNAL(zoomOut()) ); 

    connect( d->uiWidget.moveLeftButton,  SIGNAL(clicked()),
             this,                        SIGNAL(moveLeft()) ); 
    connect( d->uiWidget.moveRightButton, SIGNAL(clicked()),
             this,                        SIGNAL(moveRight()) ); 
    connect( d->uiWidget.moveUpButton,    SIGNAL(clicked()),
             this,                        SIGNAL(moveUp()) ); 
    connect( d->uiWidget.moveDownButton,  SIGNAL(clicked()),
             this,                        SIGNAL (moveDown()) ); 
}

MarbleNavigator::~MarbleNavigator()
{
    delete d;
}


int MarbleNavigator::minimumZoom() const
{
    return d->m_minimumzoom;
}


void MarbleNavigator::changeZoom( int zoom )
{
    // No infinite loops here
    // if (zoomSlider->value() != zoom)
    d->uiWidget.zoomSlider->setValue( zoom );
    d->uiWidget.zoomSlider->setMinimum( d->m_minimumzoom );
}


void MarbleNavigator::resizeEvent ( QResizeEvent * )
{
//            m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
//                                           QSizePolicy::Fixed );
    if ( height() < 100 ) {
        if ( !d->uiWidget.zoomSlider->isHidden() ) {
            d->uiWidget.zoomSlider->hide();
            d->uiWidget.m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
                                                       QSizePolicy::Expanding );
        }
    } else {
        if ( d->uiWidget.zoomSlider->isHidden() ) {
            d->uiWidget.zoomSlider->show();
            d->uiWidget.m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
                                                       QSizePolicy::Fixed );
        }
    }
} 

}

#include "moc_MarbleNavigator.cpp"
