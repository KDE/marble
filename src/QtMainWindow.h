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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H
 
#include <QtGui/QMainWindow>

#include "katlascontrol.h"

class QAction;
class QMenu;
 
class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent=0);

    private:
        void createActions();
        void createMenus();
        void createStatusBar();


        KAtlasControl *m_katlascontrol;

        QMenu *fileMenu;
        QMenu *helpMenu;

        QAction *exportMapAct;
        QAction *printAct;
        QAction *quitAct;
        QAction *copyMapAct;
        QAction *whatsThisAct;
        QAction *aboutMarbleAct;
        QAction *aboutQtAct;

    private slots:
        void exportMapScreenShot();
        void printMapScreenShot();
        void copyMap();
        void enterWhatsThis();
        void aboutMarble();
};
 
#endif
