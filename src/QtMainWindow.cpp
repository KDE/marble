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

#include "lib/katlasaboutdialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_katlascontrol = new KAtlasControl(this);

    setWindowTitle( tr("Marble - Desktop Globe") );
    setCentralWidget(m_katlascontrol);

    createActions();
    createMenus();
    createStatusBar();
}

void MainWindow::createActions()
 {
     openAct = new QAction( QIcon(":/icons/document-open.png"), tr( "&Open..."), this );
     openAct->setShortcut( tr( "Ctrl+O" ) );
     openAct->setStatusTip( tr( "Open a file for viewing on Marble"));
     connect( openAct, SIGNAL( triggered() ),
              this, SLOT( openFile() ) );

     exportMapAct = new QAction( QIcon(":/icons/document-save-as.png"), tr("&Export Map..."), this);
     exportMapAct->setShortcut(tr("Ctrl+S"));
     exportMapAct->setStatusTip(tr("Save a screenshot of the map"));
     connect(exportMapAct, SIGNAL(triggered()), this, SLOT(exportMapScreenShot()));

     printAct = new QAction( QIcon(":/icons/document-print.png"), tr("&Print..."), this);
     printAct->setShortcut(tr("Ctrl+P"));
     printAct->setStatusTip(tr("Print a screenshot of the map"));
     connect(printAct, SIGNAL(triggered()), this, SLOT(printMapScreenShot()));

     quitAct = new QAction( QIcon(":/icons/application-exit.png"), tr("&Quit"), this);
     quitAct->setShortcut(tr("Ctrl+Q"));
     quitAct->setStatusTip(tr("Quit the Application"));
     connect(quitAct, SIGNAL(triggered()), qApp, SLOT(quit()));

     copyMapAct = new QAction( QIcon(":/icons/edit-copy.png"), tr("&Copy Map"), this);
     copyMapAct->setShortcut(tr("Ctrl+C"));
     copyMapAct->setStatusTip(tr("Copy a screenshot of the map"));
     connect(copyMapAct, SIGNAL(triggered()), this, SLOT(copyMap()));

     whatsThisAct = new QAction( QIcon(":/icons/help-whatsthis.png"), tr("What's &This"), this);
     whatsThisAct->setShortcut(tr("Shift+F1"));
     whatsThisAct->setStatusTip(tr("Show a detailed explanation of the action."));
     connect(whatsThisAct, SIGNAL(triggered()), this, SLOT(enterWhatsThis()));

     aboutMarbleAct = new QAction( QIcon(":/icons/marble.png"), tr("&About Marble Desktop Globe"), this);
     aboutMarbleAct->setStatusTip(tr("Show the application's About Box"));
     connect(aboutMarbleAct, SIGNAL(triggered()), this, SLOT(aboutMarble()));

     aboutQtAct = new QAction(tr("About &Qt"), this);
     aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
     connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(exportMapAct);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    fileMenu = menuBar()->addMenu(tr("&Edit"));
    fileMenu->addAction(copyMapAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(whatsThisAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutMarbleAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
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

void MainWindow::enterWhatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

void MainWindow::aboutMarble()
{
    KAtlasAboutDialog dlg(this);
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

#include "QtMainWindow.moc"
