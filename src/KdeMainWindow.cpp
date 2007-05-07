#include "KdeMainWindow.h"

#include <KApplication>
#include <KIcon>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>
#include <KStatusBar>
 
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent) : KXmlGuiWindow(parent)
{
    m_katlascontrol = new KAtlasControl(this);
    setCentralWidget(m_katlascontrol);

    setupActions();

    createStatusBar();
}

void MainWindow::setupActions()
{
    m_copyMapAction = actionCollection()->addAction( "copyMap" );
    m_copyMapAction->setText(i18n("&Copy Map"));
    m_copyMapAction->setIcon(KIcon("edit-copy"));
    m_copyMapAction->setShortcut(Qt::CTRL+Qt::Key_C);
    connect(m_copyMapAction, SIGNAL(triggered(bool)), this, SLOT(copyMap()));

    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());
    setupGUI();
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(i18n("Ready"));
}

void MainWindow::copyMap()
{
    QPixmap mapPixmap = m_katlascontrol->mapScreenShot();

    QClipboard *clipboard = QApplication::clipboard();

    clipboard->setPixmap( mapPixmap );
}

#ifndef Q_OS_MACX
#include "KdeMainWindow.moc"
#endif
