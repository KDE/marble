#include "QtMainWindow.h"

#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_katlascontrol = new KAtlasControl(0);
    setCentralWidget(m_katlascontrol);

    createActions();
    createMenus();
    createStatusBar();
}

void MainWindow::createActions()
 {
     quitAct = new QAction( QIcon(":/icons/application-exit.png"), tr("&Quit"), this);
     quitAct->setShortcut(tr("Ctrl+Q"));
     quitAct->setStatusTip(tr("Quit the Application"));
     connect(quitAct, SIGNAL(triggered()), qApp, SLOT(quit()));

 
     aboutQtAct = new QAction(tr("About &Qt"), this);
     aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
     connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(quitAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}
#ifndef Q_OS_MACX
#include "QtMainWindow.moc"
#endif
