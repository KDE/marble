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
#include <QVariantMap>
#include "ControlView.h"

class QActionGroup;
class QAction;
class QLabel;
class QMenu;
class QProgressBar;

namespace Marble
{

class GeoDataContainer;
class MarbleWidget;
class SunControlWidget;
class TimeControlWidget;
class QtMarbleConfigDialog;
class DownloadRegionDialog;
class MovieCaptureDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString& marbleDataPath = QString(),
                        const QVariantMap& cmdLineSettings = QVariantMap(),
                        QWidget *parent=0);
    ~MainWindow();


    ControlView* marbleControl() {
        return m_controlView;
    }
    MarbleWidget* marbleWidget() {
        return m_controlView->marbleWidget();
    }

    void addGeoDataFile( const QString &fileName );

protected:
    void  closeEvent( QCloseEvent *event );

private:
    void  createActions();
    void  createMenus( const QList<QAction*> &panelActions );
    void  createStatusBar();

    static QString  readMarbleDataPath();
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

    void  changeRecordingState();

    void  updateWindowTitle();

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
    void  showMovieCaptureDialog();
    void  stopRecording();

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
    void  changeViewSize( QAction* );
    void  showFullScreen( bool );
    void  showStatusBar( bool );
    void  setupStatusBar();
    void  setupDownloadProgressBar();

    // Help Menu
    void  enterWhatsThis();
    void  aboutMarble();
    void  handbook();
    void  openForum();

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
    void changeAngleDisplayUnit( QAction *action );

    void handleProgress( int, int );
    void removeProgressItem();

private:
    ControlView *m_controlView;
    QSize m_savedSize;
    SunControlWidget* m_sunControlDialog;
    TimeControlWidget* m_timeControlDialog;
    QtMarbleConfigDialog *m_configDialog;
    DownloadRegionDialog* m_downloadRegionDialog;
    MovieCaptureDialog* m_movieCaptureDialog;

    /// Store plugin toolbar pointers so that they can be removed/updated later
    QList<QToolBar*> m_pluginToolbars;
    /// Store plugin menus so that they can be removed/updated later
    QList<QAction*> m_pluginMenus;

    QMenu *m_fileMenu;
    QMenu *m_viewMenu;
    QMenu *m_helpMenu;
    QMenu *m_settingsMenu;
    QMenu *m_panelMenu;
    QMenu *m_viewSizeMenu;
    QMenu *m_infoBoxesMenu;
    QMenu *m_onlineServicesMenu;
    QMenu *m_bookmarkMenu;
    // File Menu
    QAction *m_openAction;
    QAction *m_exportMapAction;
    QAction *m_downloadAction;
    QAction *m_downloadRegionAction;
    QAction *m_printPreviewAction;
    QAction *m_printAction;
    QAction *m_workOfflineAction;
    QAction *m_quitAction;
    QAction *m_mapWizardAction;

    // Edit Menu
    QAction *m_copyMapAction;
    QAction *m_copyCoordinatesAction;
    QAction *m_osmEditAction;
    QAction *m_recordMovieAction;
    QAction *m_stopRecordingAction;

    // View Menu
    QAction *m_showCloudsAction;
    QAction *m_controlSunAction;
    QAction *m_controlTimeAction;
    QAction *m_reloadAction;

    // Settings Menu
    QAction *m_fullScreenAction;
    QAction *m_statusBarAction;
    QAction *m_configDialogAction;
    QActionGroup *m_viewSizeActsGroup;

    // Help Menu
    QAction *m_whatsThisAction;
    QAction *m_aboutMarbleAction;
    QAction *m_aboutQtAction;
    QAction *m_lockFloatItemsAction;
    QAction *m_handbookAction;
    QAction *m_forumAction;

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
    QAction     *m_toggleTileLevelAction;
    QActionGroup *m_angleDisplayUnitActionGroup;
    QAction     *m_dmsDegreeAction;
    QAction     *m_decimalDegreeAction;
    QAction     *m_utmAction;
    void updateStatusBar();

    //Bookmark Menu
    QAction *m_addBookmarkAction;
    QAction *m_setHomeAction;
    QAction *m_toggleBookmarkDisplayAction;
    QAction *m_manageBookmarksAction;

    QString m_lastFileOpenPath;
    QStringList m_commandlineFilePaths;
};

} // namespace Marble

#endif
