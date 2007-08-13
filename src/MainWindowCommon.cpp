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

