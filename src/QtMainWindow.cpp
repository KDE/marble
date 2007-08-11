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

#include "QtMainWindow.h"

#include <QtCore/QSettings>
#include <QtGui/QCloseEvent>

#include <QtGui/QAction>
#include <QtGui/QWhatsThis>
#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include <QtGui/QPrintDialog>
#include <QtGui/QPrinter>
#include <QtGui/QPainter>

#include <QtGui/QClipboard>

#include "lib/MarbleAboutDialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_katlascontrol = new KAtlasControl(this);

    setWindowTitle( tr("Marble - Desktop Globe") );
    setCentralWidget(m_katlascontrol);

    createActions();
    createMenus();
    createStatusBar();

    readSettings();
}

void MainWindow::createActions()
 {
     m_openAct = new QAction( QIcon(":/icons/document-open.png"), tr( "&Open..."), this );
     m_openAct->setShortcut( tr( "Ctrl+O" ) );
     m_openAct->setStatusTip( tr( "Open a file for viewing on Marble"));
     connect( m_openAct, SIGNAL( triggered() ),
              this, SLOT( openFile() ) );

     m_exportMapAct = new QAction( QIcon(":/icons/document-save-as.png"), tr("&Export Map..."), this);
     m_exportMapAct->setShortcut(tr("Ctrl+S"));
     m_exportMapAct->setStatusTip(tr("Save a screenshot of the map"));
     connect(m_exportMapAct, SIGNAL(triggered()), this, SLOT(exportMapScreenShot()));

     m_printAct = new QAction( QIcon(":/icons/document-print.png"), tr("&Print..."), this);
     m_printAct->setShortcut(tr("Ctrl+P"));
     m_printAct->setStatusTip(tr("Print a screenshot of the map"));
     connect(m_printAct, SIGNAL(triggered()), this, SLOT(printMapScreenShot()));

     m_quitAct = new QAction( QIcon(":/icons/application-exit.png"), tr("&Quit"), this);
     m_quitAct->setShortcut(tr("Ctrl+Q"));
     m_quitAct->setStatusTip(tr("Quit the Application"));
     connect(m_quitAct, SIGNAL(triggered()), this, SLOT(close()));

     m_copyMapAct = new QAction( QIcon(":/icons/edit-copy.png"), tr("&Copy Map"), this);
     m_copyMapAct->setShortcut(tr("Ctrl+C"));
     m_copyMapAct->setStatusTip(tr("Copy a screenshot of the map"));
     connect(m_copyMapAct, SIGNAL(triggered()), this, SLOT(copyMap()));

     m_sideBarAct = new QAction( tr("Show &Navigation Panel"), this);
     m_sideBarAct->setShortcut(tr("F9"));
     m_sideBarAct->setCheckable( true );
     m_sideBarAct->setChecked( true );
     m_sideBarAct->setStatusTip(tr("Show Navigation Panel"));
     connect(m_sideBarAct, SIGNAL(triggered( bool )), this, SLOT( showSideBar( bool )));

     m_fullScreenAct = new QAction( tr("&Full Screen Mode"), this);
     m_fullScreenAct->setShortcut(tr("Ctrl+Shift+F"));
     m_fullScreenAct->setCheckable( true );
     m_fullScreenAct->setStatusTip(tr("Full Screen Mode"));
     connect(m_fullScreenAct, SIGNAL(triggered( bool )), this, SLOT( showFullScreen( bool )));

     m_statusBarAct = new QAction( tr("&Status Bar"), this);
     m_statusBarAct->setCheckable( true );
     m_statusBarAct->setStatusTip(tr("Show Status Bar"));
     connect(m_statusBarAct, SIGNAL(triggered( bool )), this, SLOT( showStatusBar( bool )));

     m_whatsThisAct = new QAction( QIcon(":/icons/help-whatsthis.png"), tr("What's &This"), this);
     m_whatsThisAct->setShortcut(tr("Shift+F1"));
     m_whatsThisAct->setStatusTip(tr("Show a detailed explanation of the action."));
     connect(m_whatsThisAct, SIGNAL(triggered()), this, SLOT(enterWhatsThis()));

     m_aboutMarbleAct = new QAction( QIcon(":/icons/marble.png"), tr("&About Marble Desktop Globe"), this);
     m_aboutMarbleAct->setStatusTip(tr("Show the application's About Box"));
     connect(m_aboutMarbleAct, SIGNAL(triggered()), this, SLOT(aboutMarble()));

     m_aboutQtAct = new QAction(tr("About &Qt"), this);
     m_aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
     connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

}

void MainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_openAct);
    m_fileMenu->addAction(m_exportMapAct);
    m_fileMenu->addAction(m_printAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_quitAct);

    m_fileMenu = menuBar()->addMenu(tr("&Edit"));
    m_fileMenu->addAction(m_copyMapAct);

    m_fileMenu = menuBar()->addMenu(tr("&View"));
    m_fileMenu->addAction(m_fullScreenAct);
    m_fileMenu->addAction(m_sideBarAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_statusBarAct);

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_whatsThisAct);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_aboutMarbleAct);
    m_helpMenu->addAction(m_aboutQtAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
    statusBar()->hide();
}

void MainWindow::exportMapScreenShot()
{
    QPixmap mapPixmap = m_katlascontrol->mapScreenShot();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Map"), // krazy:exclude=qclasses
                            QDir::homePath(),
                            tr("Images (*.jpg *.png)"));

    if ( !fileName.isEmpty() )
    {
        bool success = mapPixmap.save( fileName );
        if ( success == false )
        {
            QMessageBox::warning(this, tr("Marble"), // krazy:exclude=qclasses
                   tr( "An error occurred while trying to save the file.\n" ),
                   QMessageBox::Ok);
        }
    }
}

void MainWindow::printMapScreenShot()
{
    QPixmap mapPixmap = m_katlascontrol->mapScreenShot();

    QSize printSize = mapPixmap.size();

    QPrinter* printer = new QPrinter();

    QPrintDialog printDialog(printer, this);

    if (printDialog.exec() == QDialog::Accepted) {

        QRect mapPageRect = printer->pageRect();

        printSize.scale( ( printer->pageRect() ).size(), Qt::KeepAspectRatio );

        QPoint printTopLeft( mapPageRect.x() + mapPageRect.width() /2  - printSize.width() /2 ,
                             mapPageRect.y() + mapPageRect.height()/2  - printSize.height()/2 );

        QRect mapPrintRect( printTopLeft, printSize );

        QPainter painter( printer );

        painter.drawPixmap( mapPrintRect, mapPixmap, mapPixmap.rect() );

    }
}

void MainWindow::copyMap()
{
    QPixmap mapPixmap = m_katlascontrol->mapScreenShot();

    QClipboard *clipboard = QApplication::clipboard();

    clipboard->setPixmap( mapPixmap );
}

void MainWindow::showFullScreen( bool isChecked )
{
    if ( isChecked )
    {
        QWidget::showFullScreen();
    }
    else
    {
        showNormal();
    }

    m_fullScreenAct->setChecked( isChecked ); // Sync state with the GUI
}

void MainWindow::showSideBar( bool isChecked )
{
    m_katlascontrol->setSideBarShown( isChecked );

    m_sideBarAct->setChecked( isChecked ); // Sync state with the GUI
}

void MainWindow::showStatusBar( bool isChecked )
{
    if ( isChecked )
    {
        statusBar()->show();
    }
    else
    {
        statusBar()->hide();
    }

    m_statusBarAct->setChecked( isChecked ); // Sync state with the GUI
}

void MainWindow::enterWhatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

void MainWindow::aboutMarble()
{
    MarbleAboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open File"), QString(), tr("GPS Data (*.gpx);;KML (*.kml)"));

    if ( ! fileName.isNull() ) {
        QString extension = fileName.section( '.', -1 );

        if ( extension.compare( "gpx", Qt::CaseInsensitive ) == 0 ) {
            m_katlascontrol->marbleWidget()->openGpxFile( fileName );
        }
        else if ( extension.compare( "kml", Qt::CaseInsensitive ) == 0 ) {
            m_katlascontrol->marbleWidget()->addPlaceMarkFile( fileName );
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::writeSettings()
{
#ifdef Q_WS_MAC
     QSettings settings("KDE.org", "Marble Desktop Globe");
#else
     QSettings settings("KDE", "Marble Desktop Globe");
#endif

     settings.beginGroup("MainWindow");
     settings.setValue( "size", size() );
     settings.setValue( "pos", pos() );
     settings.setValue( "fullScreen", m_fullScreenAct->isChecked() );
     settings.setValue( "sideBar", m_sideBarAct->isChecked() );
     settings.setValue( "statusBar", m_statusBarAct->isChecked() );
     settings.endGroup();
}

void MainWindow::readSettings()
{
#ifdef Q_WS_MAC
     QSettings settings("KDE.org", "Marble Desktop Globe");
#else
     QSettings settings("KDE", "Marble Desktop Globe");
#endif

     settings.beginGroup("MainWindow");
     resize(settings.value("size", QSize(400, 400)).toSize());
     move(settings.value("pos", QPoint(200, 200)).toPoint());
     showFullScreen(settings.value("fullScreen", false ).toBool());
     showSideBar(settings.value("sideBar", true ).toBool());
     showStatusBar(settings.value("statusBar", false ).toBool());
     settings.endGroup();
}

#include "QtMainWindow.moc"
