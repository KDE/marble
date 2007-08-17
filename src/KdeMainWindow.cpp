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

#include "KdeMainWindow.h"

#include <QClipboard>
#include <QtGui/QLabel>
#include <QtGui/QPainter>

#include <KApplication>
#include <KIcon>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>
#include <KStatusBar>
#include <KMessageBox>
#include <KFileDialog>
#include <KToggleFullScreenAction>
#include <KPrinter>

#include "settings.h"

// #include <KPrintDialogPage>


MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
{
    m_controlView = new ControlView(this);
    setCentralWidget( m_controlView );

    setupActions();
    setXMLFile("marbleui.rc");

    // Create the statusbar and populate it with initial data.
    createStatusBar();
    connect( m_controlView->marbleWidget(), SIGNAL( zoomChanged( int ) ),
             this,                          SLOT( showZoom( int ) ) );
    showZoom( m_controlView->marbleWidget()->zoom() );

    setAutoSaveSettings();
}

MainWindow::~MainWindow()
{
    writeSettings();
}


void MainWindow::setupActions()
{
    // Action: Print Map
    m_printMapAction = KStandardAction::print( this, SLOT( printMapScreenShot() ), actionCollection() );

    // Action: Export Map
    m_exportMapAction = new KAction( this );
    actionCollection()->addAction( "exportMap", m_exportMapAction );
    m_exportMapAction->setText( i18n( "&Export Map..." ) );
    m_exportMapAction->setIcon( KIcon( "document-save-as" ) );
    m_exportMapAction->setShortcut( Qt::CTRL + Qt::Key_S );
    connect( m_exportMapAction, SIGNAL(triggered( bool ) ),
             this,              SLOT( exportMapScreenShot() ) );

    // Action: Copy Map to the Clipboard
    m_copyMapAction = KStandardAction::copy( this, SLOT( copyMap() ), actionCollection() );
    m_copyMapAction->setText( i18n( "&Copy Map" ) );
    
    // Action: Open a Gpx or a Kml File
    m_openAct = KStandardAction::open( this, SLOT( openFile() ), actionCollection() );
    m_openAct->setText( i18n( "&Open Map..." ) );
//     m_openAct->setStatusTip( tr( "Open a file for viewing on
//             Marble"));

    // Standard actions.  So far only Quit.
    KStandardAction::quit( kapp, SLOT( closeAllWindows() ), actionCollection() );

    m_sideBarAct = new KAction( i18n("Show &Navigation Panel"), this );
    actionCollection()->addAction( "options_show_sidebar", m_sideBarAct );
    m_sideBarAct->setShortcut( Qt::Key_F9 );
    m_sideBarAct->setCheckable( true );
    m_sideBarAct->setChecked( true );
    m_sideBarAct->setStatusTip( i18n( "Show Navigation Panel" ) );
    connect( m_sideBarAct, SIGNAL( triggered( bool ) ), this, SLOT( showSideBar( bool ) ) );

    m_fullScreenAct = KStandardAction::fullScreen( 0, 0, this, actionCollection() );
    connect( m_fullScreenAct, SIGNAL( triggered( bool ) ), this, SLOT( showFullScreen( bool ) ) );

    setupGUI();
}


void MainWindow::createStatusBar()
{
    // This hides the normal statusbar contents until clearMessage() is called.
    //statusBar()->showMessage( i18n( "Ready" ) );

    m_zoomLabel = new QLabel( statusBar() );
    statusBar()->addWidget(m_zoomLabel);
}


void MainWindow::saveProperties( KConfigGroup &group )
{
    Q_UNUSED( group )
}


void MainWindow::readProperties( const KConfigGroup &group )
{
    Q_UNUSED( group )
}


void MainWindow::writeSettings()
{
     double homeLon = 0;
     double homeLat = 0;
     int homeZoom = 0;
     m_controlView->marbleWidget()->home( homeLon, homeLat, homeZoom );
     MarbleSettings::setHomeLongitude( homeLon );
     MarbleSettings::setHomeLatitude( homeLat );
     MarbleSettings::setHomeZoom( homeZoom );
     MarbleSettings::self()->writeConfig();
}


void MainWindow::readSettings()
{
     m_controlView->marbleWidget()->setHome( 
        MarbleSettings::homeLongitude(),
        MarbleSettings::homeLatitude(),
        MarbleSettings::homeZoom()
     );
     m_controlView->marbleWidget()->goHome();
}


void MainWindow::showZoom(int zoom)
{
  m_zoomLabel->setText( i18n( "Zoom: %1", QString("%1").arg ( zoom, 4 ) ) );
}



void MainWindow::exportMapScreenShot()
{
    QPixmap  mapPixmap = m_controlView->mapScreenShot();

    QString  fileName = KFileDialog::getSaveFileName( QDir::homePath(),
                                                      i18n( "Images (*.jpg *.png)" ),
                                                      this, i18n("Export Map") );

    if ( !fileName.isEmpty() ) {
        bool  success = mapPixmap.save( fileName );
        if ( !success ) {
            KMessageBox::error( this, i18nc( "Application name", "Marble" ),
                                i18n( "An error occurred while trying to save the file.\n" ),
                                KMessageBox::Notify );
        }
    }
}


void MainWindow::printMapScreenShot()
{
    QPixmap       mapPixmap = m_controlView->mapScreenShot();
    QSize         printSize = mapPixmap.size();
    KPrinter      printer;

    if ( printer.setup( this ) ) {

        QRect  mapPageRect = printer.pageRect();

        printSize.scale( ( printer.pageRect() ).size(), Qt::KeepAspectRatio );

        QPoint  printTopLeft( mapPageRect.x() + mapPageRect.width() / 2
                              - printSize.width() / 2,
                              mapPageRect.y() + mapPageRect.height() / 2
                              - printSize.height() / 2 );

        QRect     mapPrintRect( printTopLeft, printSize );
        QPainter  painter( &printer );

        painter.drawPixmap( mapPrintRect, mapPixmap, mapPixmap.rect() );
    }
}

void MainWindow::openFile()
{
    QString fileName;
    fileName = KFileDialog::getOpenFileName( KUrl(),
                                    i18n("*.gpx|GPS Data\n*.kml"),
                                            this, i18n("Open File")
                                           );/*
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                            QString(), 
                            tr("GPS Data(*.gpx);;KML(*.kml)"));*/

    if ( ! fileName.isNull() ) {
        QString extension = fileName.section( '.', -1 );

        if ( extension.compare( "gpx", Qt::CaseInsensitive ) == 0 ) {
            m_controlView->marbleWidget()->openGpxFile( fileName );
        }
        else if ( extension.compare( "kml", Qt::CaseInsensitive ) == 0 ) {
            m_controlView->marbleWidget()->addPlaceMarkFile( fileName );
        }
    }
}


#include "KdeMainWindow.moc"
