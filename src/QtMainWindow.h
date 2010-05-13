//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2010 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//


#ifndef MARBLE_MAINWINDOW_H
#define MARBLE_MAINWINDOW_H
 

#include <QtGui/QMainWindow>

#include "ControlView.h"


class QAction;
class QLabel;
class QMenu;
class QPrinter;

namespace Marble
{

class MarbleWidget;
class SunControlWidget;
class QtMarbleConfigDialog;
class DownloadRegionDialog;

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

 private Q_SLOTS:
    void  showPosition( const QString& position);
    void  showDistance( const QString& position);

    void  initObject();
    void  editSettings();
    void  updateSettings();
    
    // File Menu
    void  openFile();
    void  openMapSite();
    void  exportMapScreenShot();
    void  showDownloadRegionDialog();
    void  printMapScreenShot();
    void  printPixmap( QPrinter * printer, const QPixmap& pixmap );
    void  printPreview();
    void  paintPrintPreview( QPrinter * printer );
    void  workOffline( bool );
    
    // Edit Menu
    void  copyMap();
    void  copyCoordinates();
    
    // View Menu
    void  lockPosition( bool );
    void  createInfoBoxesMenu();
    void  createOnlineServicesMenu();
    void  createPluginMenus();
    void  showClouds( bool );
    void  showAtmosphere( bool );
    void  controlSun();
    void  showSun( bool );
    void  reload();
    
    // Settings Menu
    void  showFullScreen( bool );
    void  showSideBar( bool );
    void  showStatusBar( bool );
    void  setupStatusBar();
    
    // Help Menu
    void  enterWhatsThis();
    void  aboutMarble();
    void  handbook();

    // Download region dialog
    void  connectDownloadRegionDialog();
    void  disconnectDownloadRegionDialog();
    void  downloadRegion();

 private:
    ControlView *m_controlView;
    SunControlWidget* m_sunControlDialog;
    QtMarbleConfigDialog *m_configDialog;
    DownloadRegionDialog* m_downloadRegionDialog;

    /// Store plugin toolbar pointers so that they can be removed/updated later
    QList<QToolBar*> m_pluginToolbars;
    /// Store plugin menus so that they can be removed/updated later
    QList<QMenu*> m_pluginMenus;
    
    QMenu *m_fileMenu;
    QMenu *m_helpMenu;
    QMenu *m_settingsMenu;
    QMenu *m_infoBoxesMenu;
    QMenu *m_onlineServicesMenu;

    // File Menu
    QAction *m_exportMapAct;
    QAction *m_downloadAct;
    QAction *m_downloadRegionAction;
    QAction *m_printPreviewAct;
    QAction *m_printAct;
    QAction *m_workOfflineAct;
    QAction *m_quitAct;

    // Edit Menu
    QAction *m_copyMapAct;
    QAction *m_copyCoordinatesAct;

    // View Menu
    QAction *m_showCloudsAct;
    QAction *m_showAtmosphereAct;
    QAction *m_controlSunAct;
    QAction *m_reloadAct;

    // Settings Menu
    QAction *m_sideBarAct;
    QAction *m_fullScreenAct;
    QAction *m_statusBarAct;
    QAction *m_configDialogAct;

    // Help Menu
    QAction *m_whatsThisAct;
    QAction *m_aboutMarbleAct;
    QAction *m_aboutQtAct;
    QAction *m_openAct;
    QAction *m_lockFloatItemsAct;
    QAction *m_handbookAct;

    // Status Bar
    QString     m_position;
    QString     m_distance;
    QLabel      *m_positionLabel;
    QLabel      *m_distanceLabel;    
    void updateStatusBar();
};

} // namespace Marble
 
#endif
