//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_STACKABLEWINDOW_H
#define MARBLE_STACKABLEWINDOW_H

#include <QtGui/QMainWindow>

namespace Marble
{

/**
  * A main window that sets the WA_Maemo5StackedWindow attribute
  * to serve as a stackable window on Maemo. It also emits a signal
  * when closed.
  */
class StackableWindow : public QMainWindow
{
    Q_OBJECT

public:
    /** Constructor
      * The passed parent should have the WA_Maemo5StackedWindow attribute
      * set as well such that this window will appear as a child stackable
      * window.
      */
    StackableWindow( QWidget* parent = 0, Qt::WindowFlags flags = 0 );

Q_SIGNALS:
    /** Emitted prior to the window being closed (hidden) */
    void aboutToClose();

protected:
    virtual void closeEvent ( QCloseEvent * event );
};

}

#endif
