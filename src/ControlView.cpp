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

#define DGML2 0

#include "ControlView.h"

#include <QtGui/QLayout>
#include <QtGui/QSplitter>
#include <QtGui/QStringListModel>

#include <MarbleWidget.h>
#include <MarbleModel.h>
#include <MapThemeManager.h>

ControlView::ControlView( QWidget *parent )
   : QWidget(parent)
{
    setWindowTitle(tr("Marble - Desktop Globe"));

    resize( 680, 640 );

    m_control = new MarbleControlBox( this );
    m_splitter = new QSplitter (this);

    m_marbleWidget = new MarbleWidget( this );
    m_marbleWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding,
                                                QSizePolicy::MinimumExpanding ) );
    m_marbleWidget->setDownloadUrl( "http://download.kde.org/apps/marble/" );

    QVBoxLayout *vlayout = new QVBoxLayout( this );

    m_splitter->addWidget( m_control );
    m_splitter->addWidget( m_marbleWidget );
    m_splitter->setSizes( QList<int>() << 180 << width()-180 );
    m_splitter->setStretchFactor(m_splitter->indexOf(m_control), 0);
    m_splitter->setStretchFactor(m_splitter->indexOf(m_marbleWidget), 1);

    vlayout->addWidget( m_splitter );
    vlayout->setMargin(0);

    m_control->addMarbleWidget( m_marbleWidget );

    m_mapThemeManager = new MapThemeManager;
#if DGML2
    m_control->setMapThemeModel( m_mapThemeManager->mapThemeModel() );
#endif
}

ControlView::~ControlView()
{
    delete m_mapThemeManager;
}

void ControlView::zoomIn()
{
    m_marbleWidget->zoomIn();
}

void ControlView::zoomOut()
{
    m_marbleWidget->zoomOut();
}

void ControlView::moveLeft()
{
    m_marbleWidget->moveLeft();
}

void ControlView::moveRight()
{
    m_marbleWidget->moveRight();
}

void ControlView::moveUp()
{
    m_marbleWidget->moveUp();
}

void ControlView::moveDown()
{
    m_marbleWidget->moveDown();
}

void ControlView::setSideBarShown( bool show )
{
    m_control->setVisible( show );
}

void ControlView::setNavigationTabShown( bool show )
{
    m_control->setNavigationTabShown( show );
}

void ControlView::setLegendTabShown( bool show )
{
    m_control->setLegendTabShown( show );
}

void ControlView::setMapViewTabShown( bool show )
{
    m_control->setMapViewTabShown( show );
}

void ControlView::setCurrentLocationTabShown( bool show )
{
    m_control->setCurrentLocationTabShown( show );
}

void ControlView::setFileViewTabShown( bool show )
{
    m_control->setFileViewTabShown( show );
}

#include "ControlView.moc"
