//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn <tackat@kde.org>
//

#include <config-marble.h>

#ifdef QTONLY
#include "QtMainWindow.h"
#else
#include "KdeMainWindow.h"
#endif

#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>

void MainWindow::showFullScreen( bool isChecked )
{
    if ( isChecked ) {
        QWidget::showFullScreen();
    }
    else {
        showNormal();
    }

    m_fullScreenAct->setChecked( isChecked ); // Sync state with the GUI
}

void MainWindow::showSideBar( bool isChecked )
{
    m_controlView->setSideBarShown( isChecked );

    m_sideBarAct->setChecked( isChecked ); // Sync state with the GUI
}

void MainWindow::copyMap()
{
    QPixmap      mapPixmap = m_controlView->mapScreenShot();
    QClipboard  *clipboard = QApplication::clipboard();

    clipboard->setPixmap( mapPixmap );
}

