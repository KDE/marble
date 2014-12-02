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
#include <QProgressBar>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>

// KDE
#include <kaction.h>
#include <kactioncollection.h>
#include <kparts/part.h>
#include <kparts/componentfactory.h>
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

    setXMLFile( "marbleui.rc" );

    setStandardToolBarMenuEnabled( true );

    createGUI( 0 );
    QList<QAction*> panelActions = m_part->controlView()->setupDockWidgets( this );
    m_part->readTrackingSettings();
    m_part->unplugActionList( "panels_actionlist" );
    m_part->plugActionList( "panels_actionlist", panelActions );

    // View size actions
    m_viewSizeActsGroup = new QActionGroup( this );

    QAction *actDefault = new QAction( tr( "Default (Resizable)" ), this );
    actDefault->setCheckable(true);
    m_viewSizeActsGroup->addAction(actDefault);

    QAction *actSeparator = new QAction(this);
    actSeparator->setSeparator(true);
    m_viewSizeActsGroup->addAction(actSeparator);

    QAction *actNtsc = new QAction( tr( "NTSC (720x486)" ), this );
    actNtsc->setData( QSize( 720, 486 ) );
    actNtsc->setCheckable(true);
    m_viewSizeActsGroup->addAction(actNtsc);

    QAction *actPal = new QAction( tr( "PAL (720x576)" ), this );
    actPal->setData( QSize( 720, 576 ) );
    actPal->setCheckable(true);
    m_viewSizeActsGroup->addAction(actPal);

    QAction *actNtsc16x9 = new QAction( tr( "NTSC 16:9 (864x486)" ), this );
    actNtsc16x9->setData( QSize( 864, 486 ) );
    actNtsc16x9->setCheckable(true);
    m_viewSizeActsGroup->addAction(actNtsc16x9);

    QAction *actPal16x9 = new QAction( tr( "PAL 16:9 (1024x576)" ), this );
    actPal16x9->setData( QSize( 1024, 576 ) );
    actPal16x9->setCheckable(true);
    m_viewSizeActsGroup->addAction(actPal16x9);

    QAction *actDvd = new QAction( tr( "DVD (852x480p)" ), this );
    actDvd->setData( QSize( 852, 480 ) );
    actDvd->setCheckable(true);
    m_viewSizeActsGroup->addAction(actDvd);

    QAction *actHd = new QAction( tr( "HD (1280x720p)" ), this );
    actHd->setData( QSize( 1280, 720 ) );
    actHd->setCheckable(true);
    m_viewSizeActsGroup->addAction(actHd);

    QAction *actFullhd = new QAction( tr( "Full HD (1920x1080p)" ), this );
    actFullhd->setData( QSize( 1920, 1080 ) );
    actFullhd->setCheckable(true);
    m_viewSizeActsGroup->addAction(actFullhd);

    QAction *actDc = new QAction( tr( "Digital Cinema (2048x1536)" ), this );
    actDc->setData( QSize( 2048, 1536 ) );
    actDc->setCheckable(true);
    m_viewSizeActsGroup->addAction(actDc);

    /**
     * FIXME: Needs testing, worked with errors.
    QAction *act4kuhd = new QAction( tr( "4K UHD (3840x2160)" ), this );
    act4kuhd->setData( QSize( 3840, 2160 ) );
    act4kuhd->setCheckable(true);
    m_viewSizeActsGroup->addAction(act4kuhd);

    QAction *act4k = new QAction( tr( "4K (4096x3072)" ), this );
    act4k->setData( QSize( 4096, 3072 ) );
    act4k->setCheckable(true);
    m_viewSizeActsGroup->addAction(act4k);
    */

    connect( m_viewSizeActsGroup, SIGNAL(triggered(QAction*)), this, SLOT(changeViewSize(QAction*)) );

    actDefault->setChecked( true );

    m_part->plugActionList( "viewSize_actionlist", m_viewSizeActsGroup->actions() );

    // Creating the plugin menus
    m_part->createInfoBoxesMenu();
    m_part->createOnlineServicesMenu();
    m_part->createRenderPluginActions();
	m_part->createFolderList();

    setAutoSaveSettings();

    connect( marbleWidget(), SIGNAL(themeChanged(QString)),
            this, SLOT(setMapTitle()));
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

void MainWindow::setMapTitle()
{
    GeoSceneDocument *mapTheme = marbleWidget()->mapTheme();
    if ( mapTheme ) {
        setWindowTitle(tr("Marble Virtual Globe") + " - " + mapTheme->head()->name());
    }
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

}
#include "KdeMainWindow.moc"
