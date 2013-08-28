//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2010 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2010-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#ifndef MARBLE_MAINWINDOW_H
#define MARBLE_MAINWINDOW_H


#include <QMainWindow>
#include <QDateTime>
#include <QVariantMap>
#include "ControlView.h"

class QAction;
class QLabel;
class QMenu;
class QPrinter;
class QProgressBar;

namespace Marble
{

class MarbleWidget;
class SunControlWidget;
class TimeControlWidget;
class QtMarbleConfigDialog;
class DownloadRegionDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString& marbleDataPath = QString(),
                        const QVariantMap& cmdLineSettings = QVariantMap(),
                        QWidget *parent=0);

    ControlView* marbleControl() {
        return m_controlView;
    }
    MarbleWidget* marbleWidget() {
        return m_controlView->marbleWidget();
    }

    void addGeoDataFile( const QString &fileName );

protected:
    void  closeEvent(QCloseEvent *event);

private:
    void  createActions();
    void  createMenus( const QList<QAction*> &panelActions );
    void  createStatusBar();

    QString  readMarbleDataPath();
    void  readSettings(const QVariantMap& overrideSettings = QVariantMap());
    void  writeSettings();

private Q_SLOTS:
    void  showPosition( const QString& position);
    void  showDistance( const QString& position);
    void  showZoom( int );
    void  showDateTime();

    void  initObject(const QVariantMap& cmdLineSettings);
    void  editSettings();
    void  updateSettings();
    
    void  fallBackToDefaultTheme();

    // File Menu
    void  openFile();
    void  openMapDialog();
    void  exportMapScreenShot();
    void  showDownloadRegionDialog();
    void  printMapScreenShot();
    void  workOffline( bool );
    void  showMapWizard();

    // Edit Menu
    void  copyMap();
    void  copyCoordinates();
    void  updateMapEditButtonVisibility( const QString &mapTheme );

    // View Menu
    void  lockPosition( bool );
    void  createPluginsMenus();
    void  createPluginMenus();
    void  showClouds( bool );
    void  controlSun();
    void  controlTime();
    void  showSun( bool );
    void  reload();

    // Settings Menu
    void  showFullScreen( bool );
    void  showStatusBar( bool );
    void  setupStatusBar();
    void  setupDownloadProgressBar();

    // Help Menu
    void  enterWhatsThis();
    void  aboutMarble();
    void  handbook();

    //Bookmark Menu
    void  openEditBookmarkDialog();
    void  setHome();
    void  createBookmarksListMenu( QMenu *bookmarksListMenu, const GeoDataContainer *container );
    void  lookAtBookmark( QAction * action );
    void  manageBookmarks();
    void  createBookmarkMenu();
    void  createFolderList( QMenu *bookmarksListMenu, const GeoDataContainer *container );
    void  showBookmarks( bool show );

    // Download region dialog
    void  downloadRegion();

    void showZoomLevel( bool show );

    void downloadJobAdded();
    void downloadJobRemoved();

private:
    ControlView *m_controlView;
    SunControlWidget* m_sunControlDialog;
    TimeControlWidget* m_timeControlDialog;
    QtMarbleConfigDialog *m_configDialog;
    DownloadRegionDialog* m_downloadRegionDialog;

    /// Store plugin toolbar pointers so that they can be removed/updated later
    QList<QToolBar*> m_pluginToolbars;
    /// Store plugin menus so that they can be removed/updated later
    QList<QMenu*> m_pluginMenus;

    QMenu *m_fileMenu;
    QMenu *m_viewMenu;
    QMenu *m_helpMenu;
    QMenu *m_settingsMenu;
    QMenu *m_panelMenu;
    QMenu *m_infoBoxesMenu;
    QMenu *m_onlineServicesMenu;
    QMenu *m_bookmarkMenu;
    // File Menu
    QAction *m_openAct;
    QAction *m_exportMapAct;
    QAction *m_downloadAct;
    QAction *m_downloadRegionAction;
    QAction *m_printPreviewAct;
    QAction *m_printAct;
    QAction *m_workOfflineAct;
    QAction *m_quitAct;
    QAction *m_mapWizardAct;

    // Edit Menu
    QAction *m_copyMapAct;
    QAction *m_copyCoordinatesAct;
    QAction *m_osmEditAction;

    // View Menu
    QAction *m_showCloudsAct;
    QAction *m_controlSunAct;
    QAction *m_controlTimeAct;
    QAction *m_reloadAct;

    // Settings Menu
    QAction *m_fullScreenAct;
    QAction *m_statusBarAct;
    QAction *m_configDialogAct;

    // Help Menu
    QAction *m_whatsThisAct;
    QAction *m_aboutMarbleAct;
    QAction *m_aboutQtAct;
    QAction *m_lockFloatItemsAct;
    QAction *m_handbookAct;

    // Status Bar
    QString     m_position;
    QString     m_distance;
    QString     m_zoom;
    QString     m_clock;
    QLabel      *m_positionLabel;
    QLabel      *m_distanceLabel;
    QLabel      *m_zoomLabel;
    QLabel      *m_clockLabel;
    QProgressBar *m_downloadProgressBar;
    void updateStatusBar();

    //Bookmark Menu
    QAction *m_addBookmarkAct;
    QAction *m_setHomeAct;
    QAction *m_toggleBookmarkDisplayAct;
    QAction *m_manageBookmarksAct;

    QString m_lastFileOpenPath;
    QStringList m_commandlineFilePaths;
};

} // namespace Marble

#endif
