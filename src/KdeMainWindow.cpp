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
//#include <QtGui/QMessageBox>
#include <QtGui/QLabel>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrinter>
#include <QtGui/QPainter>

#include <KApplication>
#include <KIcon>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>
#include <KStatusBar>
#include <KMessageBox>
#include <KFileDialog>


// #include <KPrinter>
// #include <KPrintDialogPage>


MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
{
    m_katlascontrol = new KAtlasControl(this);
    setCentralWidget(m_katlascontrol);

    setupActions();
    setXMLFile("marbleui.rc");

    // Create the statusbar and populate it with initial data.
    createStatusBar();
    connect( m_katlascontrol->marbleWidget(), SIGNAL( zoomChanged( int ) ),
             this,                            SLOT( showZoom( int ) ) );
    showZoom( m_katlascontrol->marbleWidget()->zoom() );
}


void MainWindow::setupActions()
{
    // Action: Print Map
    m_printMapAction = new KAction( this );
    actionCollection()->addAction( "printMap", m_printMapAction );
    m_printMapAction->setText(i18n( "&Print" ) );
    m_printMapAction->setIcon( KIcon( "document-print" ) );
    m_printMapAction->setShortcut( Qt::CTRL + Qt::Key_P );
    connect( m_printMapAction, SIGNAL( triggered( bool ) ), 
             this,             SLOT( printMapScreenShot() ) );

    // Action: Export Map
    m_exportMapAction = new KAction( this );
    actionCollection()->addAction( "exportMap", m_exportMapAction );
    m_exportMapAction->setText( i18n( "&Export map" ) );
    m_exportMapAction->setIcon( KIcon( "document-save-as" ) );
    m_exportMapAction->setShortcut( Qt::CTRL + Qt::Key_S );
    connect( m_exportMapAction, SIGNAL(triggered( bool ) ),
             this,              SLOT( exportMapScreenShot() ) );

    // Action: Copy Map to the Clipboard
    m_copyMapAction = new KAction( this );
    actionCollection()->addAction( "copyMap", m_copyMapAction );
    m_copyMapAction->setText( i18n( "&Copy Map" ) );
    m_copyMapAction->setIcon( KIcon( "edit-copy" ) );
    m_copyMapAction->setShortcut( Qt::CTRL + Qt::Key_C );
    connect( m_copyMapAction, SIGNAL( triggered( bool ) ),
             this,            SLOT( copyMap() ) );
    
    // Action: Open a Gpx or a Kml File
    m_openAct = KStandardAction::open( this, SLOT( openFile() ), actionCollection() );
    m_openAct->setText( i18n( "&Open Map" ) );
//     m_openAct->setStatusTip( tr( "Open a file for viewing on
//             Marble"));

    // Standard actions.  So far only Quit.
    KStandardAction::quit( kapp, SLOT( quit() ), actionCollection() );
    setupGUI();
}


void MainWindow::createStatusBar()
{
    // This hides the normal statusbar contents until clearMessage() is called.
    //statusBar()->showMessage( i18n( "Ready" ) );

    m_zoomLabel = new QLabel();
    statusBar()->addWidget(m_zoomLabel);
}


void MainWindow::showZoom(int zoom)
{
  m_zoomLabel->setText( i18n( "Zoom: %1", QString("%1").arg ( zoom, 4 ) ) );
}



void MainWindow::exportMapScreenShot()
{
    QPixmap  mapPixmap = m_katlascontrol->mapScreenShot();

    QString  fileName = KFileDialog::getSaveFileName( QDir::homePath(),
                                                      i18n( "Images (*.jpg *.png)" ),
                                                      this, i18n("Export Map") );

    if ( !fileName.isEmpty() ) {
        bool  success = mapPixmap.save( fileName );
        if ( !success ) {
            KMessageBox::error( this, i18nc( "Appname", "Marble" ),
                                i18n( "An error occurred while trying to save the file.\n" ),
                                KMessageBox::Notify );
        }
    }
}


void MainWindow::printMapScreenShot()
{
    QPixmap       mapPixmap = m_katlascontrol->mapScreenShot();
    QSize         printSize = mapPixmap.size();
    QPrinter     *printer = new QPrinter();
    QPrintDialog  printDialog( printer, this );

    if ( printDialog.exec() == QDialog::Accepted ) {

        QRect  mapPageRect = printer->pageRect();

        printSize.scale( ( printer->pageRect() ).size(), Qt::KeepAspectRatio );

        QPoint  printTopLeft( mapPageRect.x() + mapPageRect.width() / 2
                              - printSize.width() / 2,
                              mapPageRect.y() + mapPageRect.height() / 2
                              - printSize.height() / 2 );

        QRect     mapPrintRect( printTopLeft, printSize );
        QPainter  painter( printer );

        painter.drawPixmap( mapPrintRect, mapPixmap, mapPixmap.rect() );
    }
}


void MainWindow::copyMap()
{
    QPixmap      mapPixmap = m_katlascontrol->mapScreenShot();
    QClipboard  *clipboard = QApplication::clipboard();

    clipboard->setPixmap( mapPixmap );
}

void MainWindow::openFile()
{
    QString fileName;
    fileName = KFileDialog::getOpenFileName( KUrl(),
                                    QString("*.gpx|GPS Data\n *.kml"),
                                            this, tr("Open File")
                                           );/*
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                            QString(), 
                            tr("GPS Data(*.gpx);;KML(*.kml)"));*/

    if ( ! fileName.isNull() ) {
        QString extension = fileName.section( '.', -1 );

        if ( extension.compare( "gpx", Qt::CaseInsensitive ) == 0 ) {
            m_katlascontrol->marbleWidget()->openGpxFile( fileName );
        }
        else if ( extension.compare( "kml", Qt::CaseInsensitive ) 
                  == 0 ) 
        {
            m_katlascontrol->marbleWidget()->addPlaceMarkFile( 
                                          fileName);
        }
    }
}


#include "KdeMainWindow.moc"
