//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

// Own
#include "KdeMainWindow.h"

// Qt
#include <QAction>
#include <QActionGroup>
#include <QCoreApplication>
#include <QCloseEvent>

// KF
#include <klocalizedstring.h>
#include <kactioncollection.h>
#include <kparts/part.h>
#include <kxmlguifactory.h>

// GeoData
#include <GeoSceneDocument.h>
#include <GeoSceneHead.h>

// Local dir
#include "MarbleDebug.h"
#include "ControlView.h"
#include "marble_part.h"

namespace Marble
{

MainWindow::MainWindow( const QString& marbleDataPath, QWidget *parent )
    : KXmlGuiWindow( parent ),
      m_savedSize( QSize( -1, -1 ) )
{
    m_part = new MarblePart( this, this, QVariantList() << marbleDataPath );

    setCentralWidget( m_part->widget() );

    insertChildClient( m_part );

    setupGUI( Default, "marbleui.rc" );
    setStandardToolBarMenuEnabled( true );

    QList<QAction*> panelActions = m_part->controlView()->setupDockWidgets( this );
    m_part->readTrackingSettings();
    m_part->unplugActionList( "panels_actionlist" );
    m_part->plugActionList( "panels_actionlist", panelActions );

    // View size actions
    m_viewSizeActsGroup = ControlView::createViewSizeActionGroup( this );
    connect( m_viewSizeActsGroup, SIGNAL(triggered(QAction*)), this, SLOT(changeViewSize(QAction*)) );

    m_part->plugActionList( "viewSize_actionlist", m_viewSizeActsGroup->actions() );

    // Creating the plugin menus
    m_part->createInfoBoxesMenu();
    m_part->createOnlineServicesMenu();
    m_part->createRenderPluginActions();
	m_part->createFolderList();

    setAutoSaveSettings();

    connect( marbleWidget(), SIGNAL(themeChanged(QString)),
            this, SLOT(updateWindowTitle()));
    updateWindowTitle();
}

MainWindow::~MainWindow()
{
    factory()->removeClient( m_part );
    delete m_part;
}

ControlView* MainWindow::marbleControl() const
{
    return m_part->controlView();
}

MarbleWidget* MainWindow::marbleWidget() const
{
    return m_part->controlView()->marbleWidget();
}

void MainWindow::updateWindowTitle()
{
    GeoSceneDocument *mapTheme = marbleWidget()->mapTheme();
    setWindowTitle(mapTheme ? mapTheme->head()->name() : QString());
}

void MainWindow::changeViewSize( QAction* action )
{
    mDebug()<<size();
    mDebug()<<minimumSize()<<maximumSize();
    if ( action->data().type() == QVariant::Size ) {
        if ( m_savedSize.isEmpty() ) {
            m_savedSize = marbleControl()->size();
        }
        marbleControl()->setFixedSize( action->data().toSize() );
        adjustSize();
    } else {
        marbleControl()->setMinimumSize( QSize( 0, 0 ) );
        marbleControl()->setMaximumSize( QSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX ) );
        marbleControl()->resize( m_savedSize );
        marbleControl()->setMinimumSize( m_savedSize );
        adjustSize();
        marbleControl()->setMinimumSize( QSize( 0, 0 ) );
        m_savedSize.setHeight( -1 );
    }
    mDebug()<<marbleControl()->size();
    mDebug()<<size();
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    QCloseEvent newEvent;
    QCoreApplication::sendEvent( this->marbleControl(), &newEvent );

    if ( newEvent.isAccepted() ) {
        event->accept();
    } else {
        event->ignore();
    }
}

}
#include "moc_KdeMainWindow.cpp"
