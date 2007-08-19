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

#include "ControlView.h"


class QAction;
class QMenu;

class KAtlasControl;
class MarbleWidget;
 

class MainWindow : public QMainWindow
{
    Q_OBJECT

 public:
    explicit MainWindow(const QString& marbleDataPath = QString(),
                        QWidget *parent=0);

    ControlView* marbleControl(){ return m_controlView; }
    MarbleWidget* marbleWidget(){ return m_controlView->marbleWidget(); }

 protected:
    void closeEvent(QCloseEvent *event);

 private:
    void createActions();
    void createMenus();
    void createStatusBar();

    QString  readMarbleDataPath();
    void  readSettings();
    void  writeSettings();

 private Q_SLOTS:
    void exportMapScreenShot();
    void printMapScreenShot();
    void copyMap();
    void showFullScreen( bool );
    void showSideBar( bool );
    void showStatusBar( bool );
    void enterWhatsThis();
    void aboutMarble();
    void openFile();

 private:
    ControlView *m_controlView;

    QMenu *m_fileMenu;
    QMenu *m_helpMenu;

    QAction *m_exportMapAct;
    QAction *m_printAct;
    QAction *m_quitAct;
    QAction *m_copyMapAct;
    QAction *m_sideBarAct;
    QAction *m_fullScreenAct;
    QAction *m_statusBarAct;
    QAction *m_whatsThisAct;
    QAction *m_aboutMarbleAct;
    QAction *m_aboutQtAct;
    QAction *m_openAct;
};

 
#endif
