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


#include "ControlView.h"

#include <QtGui/QLayout>
#include <QtGui/QSplitter>
#include <QtGui/QStringListModel>

#include <MarbleWidget.h>
#include <MarbleModel.h>
#include <MapThemeManager.h>

namespace Marble
{

ControlView::ControlView( QWidget *parent )
   : QWidget(parent)
{
    setWindowTitle(tr("Marble - Desktop Globe"));

    resize( 680, 640 );

    QVBoxLayout *vlayout = new QVBoxLayout( this );
    vlayout->setMargin(0);

    m_splitter = new QSplitter (this);
    vlayout->addWidget( m_splitter );

    m_control = new MarbleControlBox( this );
    m_splitter->addWidget( m_control );
    m_splitter->setStretchFactor(m_splitter->indexOf(m_control), 0);

    m_marbleWidget = new MarbleWidget( this );
    m_marbleWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding,
                                                QSizePolicy::MinimumExpanding ) );

    m_splitter->addWidget( m_marbleWidget );
    m_splitter->setStretchFactor(m_splitter->indexOf(m_marbleWidget), 1);
    m_splitter->setSizes( QList<int>() << 180 << width()-180 );

    m_control->addMarbleWidget( m_marbleWidget );

    m_mapThemeManager = new MapThemeManager;

    m_control->setMapThemeModel( m_mapThemeManager->mapThemeModel() );
    m_control->updateMapThemeView();
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

}

#include "ControlView.moc"
