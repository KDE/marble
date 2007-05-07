#include "QtMainWindow.h"

#include <QtGui/QAction>
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

#include "katlasaboutdialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_katlascontrol = new KAtlasControl(0);

    setWindowTitle( tr("Marble - Desktop Globe") );
    setCentralWidget(m_katlascontrol);

    createActions();
    createMenus();
    createStatusBar();
}

void MainWindow::createActions()
 {
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
    fileMenu->addAction(exportMapAct);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    fileMenu = menuBar()->addMenu(tr("&Edit"));
    fileMenu->addAction(copyMapAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
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

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Map"),
                            QDir::homePath(),
                            tr("Images (*.jpg *.png)"));

    if ( !fileName.isEmpty() )
    {
        bool success = mapPixmap.save( fileName );
        if ( success == false )
        {
            QMessageBox::warning(this, tr("Marble"),
                   tr( "An error occured while trying to save the file.\n" ),
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

void MainWindow::aboutMarble()
{
    KAtlasAboutDialog dlg(this);
    dlg.exec();
}

#ifndef Q_OS_MACX
#include "QtMainWindow.moc"
#endif
