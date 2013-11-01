//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "StackableWindow.h"

namespace Marble {

StackableWindow::StackableWindow( QWidget *parent, Qt::WindowFlags flags ) :
    QMainWindow( parent, flags )
{
#ifdef Q_WS_MAEMO_5
    setAttribute( Qt::WA_Maemo5StackedWindow );
#endif // Q_WS_MAEMO_5
    setWindowFlags( windowFlags() | Qt::Window );
}

void StackableWindow::closeEvent ( QCloseEvent *event )
{
    emit aboutToClose();
    QWidget::closeEvent( event );
}

}

#include "StackableWindow.moc"
