//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "katlascontrol.h"

#include <QtGui/QLayout>
#include <QtGui/QStringListModel>

#include <lib/MarbleWidget.h>
#include <lib/MarbleModel.h>
#include "MarbleControlBox.h"


KAtlasControl::KAtlasControl(QWidget *parent)
   : QWidget(parent)
{
    setWindowTitle(tr("Marble - Desktop Globe"));
    //	resize(640, 640);
    resize( 680, 640 );

    m_control = new MarbleControlBox( this );
    m_control->setFixedWidth( 185 );

    // Create the Model (Globe) and one view.
    m_marbleModel = new MarbleModel( this );
    m_marbleWidget = new MarbleWidget( m_marbleModel, this );
    m_marbleWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding,
                                                QSizePolicy::MinimumExpanding ) );
    m_marbleWidget->setMinimumZoom( m_control->minimumZoom() );

    QVBoxLayout *vlayout = new QVBoxLayout( this );

    QHBoxLayout *hlayout = new QHBoxLayout();	

    hlayout->addWidget( m_control );
    hlayout->addWidget( m_marbleWidget );
    vlayout->addLayout( hlayout );
    vlayout->setMargin(0);

    m_control->setLocations( m_marbleWidget->placeMarkModel() );

    connect(m_control, SIGNAL(goHome()),         m_marbleWidget, SLOT(goHome()));
    connect(m_control, SIGNAL(zoomChanged(int)), m_marbleWidget, SLOT(zoomView(int)));
    connect(m_control, SIGNAL(zoomIn()),  m_marbleWidget, SLOT(zoomIn()));
    connect(m_control, SIGNAL(zoomOut()), m_marbleWidget, SLOT(zoomOut()));

    connect(m_control, SIGNAL(moveLeft()),  m_marbleWidget, SLOT(moveLeft()));
    connect(m_control, SIGNAL(moveRight()), m_marbleWidget, SLOT(moveRight()));
    connect(m_control, SIGNAL(moveUp()),    m_marbleWidget, SLOT(moveUp()));
    connect(m_control, SIGNAL(moveDown()),  m_marbleWidget, SLOT(moveDown()));

    connect(m_marbleWidget, SIGNAL(zoomChanged(int)), 
	    m_control,      SLOT(changeZoom(int)));
    connect(m_control,      SIGNAL(centerOn(const QModelIndex&)),
	    m_marbleWidget, SLOT(centerOn(const QModelIndex&)));
    connect(m_control,      SIGNAL(selectMapTheme(const QString&)),
	    m_marbleWidget, SLOT(setMapTheme(const QString&)));
}


void KAtlasControl::zoomIn()
{
    m_marbleWidget->zoomIn();
}

void KAtlasControl::zoomOut()
{
    m_marbleWidget->zoomOut();
}

void KAtlasControl::moveLeft()
{
    m_marbleWidget->moveLeft();
}

void KAtlasControl::moveRight()
{
    m_marbleWidget->moveRight();
}

void KAtlasControl::moveUp()
{
    m_marbleWidget->moveUp();
}

void KAtlasControl::moveDown()
{
    m_marbleWidget->moveDown();
}


#ifndef Q_OS_MACX
#include "katlascontrol.moc"
#endif
