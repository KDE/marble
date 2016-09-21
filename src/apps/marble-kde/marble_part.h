//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig  <tokoe@kde.org>
// Copyright 2008      Inge Wallin    <inge@lysator.liu.se>
// Copyright 2009      Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2010      Harshit Jain   <hjain.itbhu@gmail.com>
//

#ifndef MARBLE_MARBLEPART_H
#define MARBLE_MARBLEPART_H

#include <kparts/readonlypart.h>
#include <QHash>
#include "MarbleGlobal.h"
#include <krecentfilesaction.h>
#include <QDomNode>

#include "ui_MarbleCloudSyncSettingsWidget.h"

class KAboutData;
class QAction;
class KToggleAction;
class KConfigDialog;
class KToolBar;

class QLabel;
class QProgressBar;

namespace KParts
{
class StatusBarExtension;
}

namespace Marble
{

class MovieCaptureDialog;
class ControlView;
class DownloadRegionDialog;
class SunControlWidget;
class TimeControlWidget;
class GeoDataFolder;

class MarblePart: public KParts::ReadOnlyPart
{
  Q_OBJECT

  public:
    MarblePart( QWidget *parentWidget, QObject *parent, const QVariantList& );
    virtual ~MarblePart();

    ControlView *controlView() const;

    static KAboutData* createAboutData();
    void createInfoBoxesMenu();
    void createOnlineServicesMenu();
    void createRenderPluginActions();

    void readTrackingSettings();

    void initializeCustomTimezone();

  public Q_SLOTS:
    bool  openUrl( const QUrl &url );
    bool  openFile();
    void  showPosition( const QString& position);
    void  showZoomLevel( const int );
    void  showDateTime();
    void  mapThemeChanged( const QString& newMapTheme );
    void  createPluginMenus();
    void  createFolderList();
    void  fallBackToDefaultTheme();

  private Q_SLOTS:
    void  exportMapScreenShot();
    void  printMapScreenShot();
    void  copyMap();
    void  copyCoordinates();
    void  setShowClouds( bool );
    void  setShowBookmarks( bool isChecked );
    void  showFullScreen( bool );
    void  showStatusBar( bool );
    void  changeRecordingState();

    /**
     * @brief Show the dateTime label in the status bar.
     *        This slot is connected with the "triggered" signal of
     *        m_showDateTimeAction.
     */
    void showDateTimeLabel( bool isChecked );

    /**
     * @brief Show the position label in the status bar.
     *        This slot is connected with the "triggered" signal of
     *        m_showPositionAction.
     */
    void showPositionLabel( bool isChecked );

    /**
     * @brief Show the altitude label in the status bar.
     *        This slot is connected with the "triggered" signal of
     *        m_showAltitudeAction.
     */
    void showAltitudeLabel( bool isChecked );

    /**
     * @brief Show the tile zoom level label in the status bar.
     *        This slot is connected with the "triggered" signal of
     *        m_showTileZoomLevelAction.
     */
    void showTileZoomLevelLabel( bool isChecked );

    /**
     * @brief Show the download progress bar in the status bar.
     *        This slot is connected with the "triggered" signal of
     *        m_showDownloadProgressAction.
     */
    void showDownloadProgressBar( bool isChecked );

    void  handleProgress( int active, int queued);
    void  removeProgressItem();

    void  lockFloatItemPosition( bool );
    void  controlSun();
    void  controlTime();
    void  showSun( bool );
    void  lockToSubSolarPoint( bool );
    void  setSubSolarPointIconVisible( bool );
    void  workOffline( bool );

    void  setupStatusBar();
    void  showNewStuffDialog();
    void  showUploadNewStuffDialog();
    void  showDownloadRegionDialog();
    void  downloadRegion();
    void  showStatusBarContextMenu( const QPoint& pos );
    void  showMapWizard();

    void  editSettings();

    void  enableApplyButton();
    void  applyPluginState();

    void  updateSettings();

    void  updateStatusBar();

    void updateCloudSyncStatus(const QString &status);

    /**
     * Saves the settings of all plugins.
     */
    void writePluginSettings();

    /**
     * Reads the settings of all plugins.
     */
    void readPluginSettings();

    //Bookmark Menu
    void  openEditBookmarkDialog();
    void  setHome();
    void  openManageBookmarksDialog();
    void  createBookmarksListMenu( QMenu *, const GeoDataFolder& );
    void  lookAtBookmark( QAction * );
//    void  createBookmarkMenu();

    void  updateMapEditButtonVisibility( const QString &mapTheme );
    void  showMovieCaptureDialog();
    void  stopRecording();
    void  updateCloudSyncCredentials();

  private:
    void  setupActions();
    void  setupDownloadProgressBar();
    void  setupStatusBarActions();
    QLabel * setupStatusBarLabel( const QString& templateString );

    void  readSettings();
    void  readStatusBarSettings();
    void  writeSettings();
    void  writeStatusBarSettings();

    /** Only updates member variable m_tileZoomLevel, does not trigger screen
        update. */
    void  updateTileZoomLevel();
    void migrateNewstuffConfigFiles() const;
    static void repairNode( QDomNode node, const QString &child );

  private:
    // All the functionality is provided by this widget.
    ControlView       *m_controlView;
    SunControlWidget  *m_sunControlDialog;
    TimeControlWidget *m_timeControlDialog;
    DownloadRegionDialog *m_downloadRegionDialog;
    MovieCaptureDialog *m_movieCaptureDialog;

    // Actions for the GUI.
    QAction      *m_exportMapAction;
    QAction      *m_printMapAction;
    QAction      *m_printPreviewAction;
    QAction      *m_workOfflineAction;
    QAction      *m_copyMapAction;
    QAction      *m_copyCoordinatesAction;
    QAction      *m_showCloudsAction;
    QAction      *m_fullScreenAct;
    QAction      *m_openAct;
    QAction      *m_newStuffAction;
    QAction      *m_downloadRegionAction;
    QAction      *m_controlSunAction;
    QAction      *m_controlTimeAction;
    QAction      *m_lockFloatItemsAct;
    QAction      *m_mapWizardAct;
    QAction      *m_externalMapEditorAction;
    QAction      *m_recordMovieAction;
    QAction      *m_stopRecordingAction;
    KRecentFilesAction *m_recentFilesAction;

    //Bookmark Menu
    QAction *m_addBookmarkAction;
    QAction *m_toggleBookmarkDisplayAction;
    QAction *m_setHomeAction;
    QAction *m_manageBookmarksAction;
    // Actions for the status bar
    QAction      *m_showPositionAction;
    QAction      *m_showDateTimeAction;
    QAction      *m_showAltitudeAction;
    QAction      *m_showTileZoomLevelAction;
    QAction      *m_showDownloadProgressAction;

    // Action for the tool bar
    KToggleAction *m_showShadow;
    KToggleAction *m_lockToSubSolarPoint;
    KToggleAction *m_setSubSolarPointIconVisible;

    KConfigDialog *m_configDialog;

    QHash<QString, int> m_pluginEnabled;

    QString m_position;
    QString m_clock;
    QString m_tileZoomLevel;
    QString m_lastFileOpenPath;

    // Items for the statusbar.
    QLabel       *m_positionLabel;
    QLabel       *m_clockLabel;
    QLabel       *m_distanceLabel;
    QLabel       *m_tileZoomLevelLabel;
    QProgressBar *m_downloadProgressBar;

    KParts::StatusBarExtension *m_statusBarExtension;

    QHash< int, int > m_timezone;
    QMap<int, QString> m_externalEditorMapping;
    Ui_MarbleCloudSyncSettingsWidget m_ui_cloudSyncSettings;
};

}

#endif
