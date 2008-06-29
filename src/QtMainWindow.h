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
class QLabel;
class QMenu;

class MarbleWidget;
class SunControlWidget;
class MarbleAbstractFloatItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

 public:
    explicit MainWindow(const QString& marbleDataPath = QString(),
                        QWidget *parent=0);

    ControlView* marbleControl(){ return m_controlView; }
    MarbleWidget* marbleWidget(){ return m_controlView->marbleWidget(); }

 protected:
    void  closeEvent(QCloseEvent *event);

 private:
    void  createActions();
    void  createMenus();
    void  createStatusBar();

    QString  readMarbleDataPath();
    void  readSettings();
    void  writeSettings();

 public Q_SLOTS:
    void  showPosition( const QString& position);
    void  showDistance( const QString& position);

 private Q_SLOTS:
    void  exportMapScreenShot();
    void  printMapScreenShot();
    void  copyMap();
    void  copyCoordinates();
    void  showFullScreen( bool );
    void  showSideBar( bool );
    void  showStatusBar( bool );
    void  showClouds( bool );
    void  showAtmosphere( bool );
    void  showSun();
    void  enterWhatsThis();
    void  aboutMarble();
    void  openFile();
    void  setupStatusBar();

 private:
    ControlView *m_controlView;
    SunControlWidget* m_sunControlDialog;

    QMenu *m_fileMenu;
    QMenu *m_helpMenu;
    QMenu *m_infoBoxesMenu;

    QAction *m_exportMapAct;
    QAction *m_printAct;
    QAction *m_quitAct;
    QAction *m_copyMapAct;
    QAction *m_copyCoordinatesAct;
    QAction *m_sideBarAct;
    QAction *m_fullScreenAct;
    QAction *m_statusBarAct;
    QAction *m_showCloudsAct;
    QAction *m_showAtmosphereAct;
    QAction *m_showSunAct;
    QAction *m_whatsThisAct;
    QAction *m_aboutMarbleAct;
    QAction *m_aboutQtAct;
    QAction *m_openAct;

    QString m_position;
    QString m_distance;

    // Zoom label for the statusbar.
    QLabel       *m_positionLabel;
    QLabel       *m_distanceLabel;

    void createInfoBoxesMenu();
    void updateStatusBar();
};

 
#endif
