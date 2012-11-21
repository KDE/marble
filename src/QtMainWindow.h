//
// This file is part of the Marble Virtual Globe.
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
#include <QtCore/QDateTime>
#include <QtCore/QVariantMap>
#include "ControlView.h"

class QAction;
class QLabel;
class QMenu;
class QPrinter;

namespace Marble
{

class MarbleWidget;
class SunControlWidget;
class TimeControlWidget;
class QtMarbleConfigDialog;
class DownloadRegionDialog;
class RoutingWidget;
class StackableWindow;
class GoToDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

#ifdef Q_WS_MAEMO_5
    enum Orientation {
        OrientationAutorotate,
        OrientationLandscape,
        OrientationPortrait
    };
#endif

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

#ifdef Q_WS_MAEMO_5
    Orientation orientation() const;
#endif

protected:
    void  closeEvent(QCloseEvent *event);

private:
    void  createToolBar();
    void  createActions();
    void  createMenus();
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

    // File Menu
    void  openFile();
    void  openMapSite();
    void  exportMapScreenShot();
    void  showDownloadRegionDialog();
    void  printMapScreenShot();
    void  workOffline( bool );
    void  toggleKineticScrolling( bool );
    void  showMapWizard();

    // Edit Menu
    void  copyMap();
    void  copyCoordinates();
    void  updateMapEditButtonVisibility( const QString &mapTheme );

    // View Menu
    void  lockPosition( bool );
    void  createInfoBoxesMenu();
    void  createOnlineServicesMenu();
    void  createPluginMenus();
    void  showClouds( bool );
    void  showAtmosphere( bool );
    void  controlSun();
    void  controlTime();
    void  showSun( bool );
    void  reload();

    // Settings Menu
    void  showFullScreen( bool );
#ifdef Q_WS_MAEMO_5
    void  setOrientation( Orientation orientation );
#endif
    void  showSideBar( bool );
    void  showStatusBar( bool );
    void  setupStatusBar();

    // Help Menu
    void  enterWhatsThis();
    void  aboutMarble();
    void  handbook();

    //Bookmark Menu
    void  openEditBookmarkDialog();
    void  setHome();
    void  createBookmarksListMenu(QMenu *m_bookmarksListMenu, const GeoDataFolder &folder);
    void  lookAtBookmark( QAction * action );
    void  manageBookmarks();
    void  createBookmarkMenu();
    void  createFolderList();
    void  showBookmarks( bool show );

    // Download region dialog
    void  connectDownloadRegionDialog();
    void  disconnectDownloadRegionDialog();
    void  downloadRegion();

    // Small screen devices specific slots
    void showMapViewDialog();
    void showLegendTab( bool enabled );
    void showRoutingDialog();
    void showTrackingDialog();
    void showGoToDialog();

    void showZoomLevel( bool show );

private:
    void setupZoomButtons();
    void initializeTrackingWidget();

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
    QMenu *m_helpMenu;
    QMenu *m_settingsMenu;
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
    QAction *m_kineticScrollingAction;
    QAction *m_quitAct;
    QAction *m_mapWizardAct;

    // Edit Menu
    QAction *m_copyMapAct;
    QAction *m_copyCoordinatesAct;
    QAction *m_osmEditAction;

    // View Menu
    QAction *m_showCloudsAct;
    QAction *m_showAtmosphereAct;
    QAction *m_controlSunAct;
    QAction *m_controlTimeAct;
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
    void updateStatusBar();

    //Bookmark Menu
    QAction *m_addBookmarkAct;
    QAction *m_setHomeAct;
    QAction *m_toggleBookmarkDisplayAct;
    QAction *m_manageBookmarksAct;

    // Small screen devices
    QAction *m_showMapViewDialogAction;
    QAction *m_toggleLegendTabAction;
    QAction *m_toggleRoutingTabAction;
    QAction *m_showTrackingDialogAction;

    QDialog *m_mapViewWindow;
    StackableWindow *m_routingWindow;
    StackableWindow *m_trackingWindow;
    GoToDialog *m_gotoDialog;

    RoutingWidget *m_routingWidget;

    QString m_lastFileOpenPath;
};

} // namespace Marble

#endif
