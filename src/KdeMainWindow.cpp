#include "KdeMainWindow.h"

#include <KApplication>
#include <KAction>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>
#include <KStatusBar>
 
MainWindow::MainWindow(QWidget *parent) : KMainWindow(parent)
{
    m_katlascontrol = new KAtlasControl(0);
    setCentralWidget(m_katlascontrol);

    setupActions();

    createStatusBar();
}

void MainWindow::setupActions()
{
    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());
    setupGUI();
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(i18n("Ready"));
}

#ifndef Q_OS_MACX
#include "KdeMainWindow.moc"
#endif
