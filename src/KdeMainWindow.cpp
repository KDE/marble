#include "KdeMainWindow.h"

#include <KApplication>
#include <KIcon>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>
#include <KStatusBar>
 
#include <KFileDialog>
#include <QtGui/QMessageBox>

// #include <KPrinter>
// #include <KPrintDialogPage>

#include <QtGui/QPrintDialog>
#include <QtGui/QPrinter>
#include <QtGui/QPainter>

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
    m_printMapAction = new KAction( this);
    actionCollection()->addAction("printMap", m_printMapAction );
    m_printMapAction->setText(i18n("&Print"));
    m_printMapAction->setIcon(KIcon("document-print"));
    m_printMapAction->setShortcut(Qt::CTRL+Qt::Key_P);
    connect(m_printMapAction, SIGNAL(triggered(bool)), this, SLOT(printMapScreenShot()));

    m_exportMapAction = new KAction( this);
    actionCollection()->addAction("exportMap", m_exportMapAction );
    m_exportMapAction->setText(i18n("&Export map"));
    m_exportMapAction->setIcon(KIcon("document-save-as"));
    m_exportMapAction->setShortcut(Qt::CTRL+Qt::Key_S);
    connect(m_exportMapAction, SIGNAL(triggered(bool)), this, SLOT(exportMapScreenShot()));

    m_copyMapAction = new KAction( this);
    actionCollection()->addAction("copyMap", m_copyMapAction );
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

void MainWindow::exportMapScreenShot()
{
    QPixmap mapPixmap = m_katlascontrol->mapScreenShot();

    QString fileName = KFileDialog::getSaveFileName( QDir::homePath(), i18n("Images (*.jpg *.png)"), this, i18n("Export Map") );

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

#ifndef Q_OS_MACX
#include "KdeMainWindow.moc"
#endif
