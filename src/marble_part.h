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

#include <kparts/part.h>
#include <QtCore/QHash>
#include <QtCore/QDateTime>
#include <kmenu.h>
#include <kurl.h>
#include "MarbleGlobal.h"

class KAboutData;
class KAction;
class KToggleAction;
class KConfigDialog;
class KToolBar;

class QLabel;
class QPrinter;
class QProgressBar;
class QStandardItemModel;

namespace KParts
{
class StatusBarExtension;
}

namespace Marble
{

class ControlView;
class DownloadRegionDialog;
class RoutingProfilesWidget;
class SunControlWidget;
class TimeControlWidget;
class GeoDataFolder;
class RenderPluginModel;
class SearchInputWidget;

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
    void setupToolBar( KToolBar* toolBar );

    void initializeCustomTimezone();

  public Q_SLOTS:
    bool  openUrl( const KUrl &url );
    bool  openFile();
    void  showPosition( const QString& position);
    void  showZoomLevel( const int );
    void  showDateTime();
    void  mapThemeChanged( const QString& newMapTheme );
    void  createPluginMenus();
    void  createFolderList();

  private Q_SLOTS:
    void  exportMapScreenShot();
    void  printMapScreenShot();
    void  copyMap();
    void  copyCoordinates();
    void  setShowClouds( bool );
    void  setShowAtmosphere( bool );
    void  setShowCurrentLocation( bool );
    void  setShowBookmarks( bool isChecked );
    void  showFullScreen( bool );
    void  showSideBar( bool );
    void  showStatusBar( bool );

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

    void  downloadJobAdded();
    void  downloadJobRemoved();

    void  lockFloatItemPosition( bool );
    void  controlSun();
    void  controlTime();
    void  showSun( bool );
    void  showSunInZenith( bool );
    void  workOffline( bool );

    void  setupStatusBar();
    void  showNewStuffDialog();
    void  showUploadNewStuffDialog();
    void  connectDownloadRegionDialog();
    void  disconnectDownloadRegionDialog();
    void  showDownloadRegionDialog();
    void  downloadRegion();
    void  showStatusBarContextMenu( const QPoint& pos );
    void  showMapWizard();

    void  editSettings();

    void  enableApplyButton();
    void  applyPluginState();

    void  updateSettings();

    void  updateStatusBar();

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

  private:
    // All the functionality is provided by this widget.
    ControlView       *m_controlView; // MarbleControlBox and MarbleWidget
    SunControlWidget  *m_sunControlDialog;
    TimeControlWidget *m_timeControlDialog;
    DownloadRegionDialog *m_downloadRegionDialog;
    SearchInputWidget* m_searchField;

    // Actions for the GUI.
    KAction      *m_exportMapAction;
    KAction      *m_printMapAction;
    KAction      *m_printPreviewAction;
    KAction      *m_workOfflineAction;
    KAction      *m_copyMapAction;
    KAction      *m_copyCoordinatesAction;
    KAction      *m_currentLocationAction;
    KAction      *m_showCloudsAction;
    KAction      *m_showAtmosphereAction;
    KAction      *m_sideBarAct;
    KAction      *m_fullScreenAct;
    KAction      *m_openAct;
    KAction      *m_newStuffAction;
    KAction      *m_downloadRegionAction;
    KAction      *m_controlSunAction;
    KAction      *m_controlTimeAction;
    KAction      *m_lockFloatItemsAct;
    KAction      *m_mapWizardAct;
    KAction      *m_externalMapEditorAction;

    //Bookmark Menu
    KAction *m_addBookmarkAction;
    KAction *m_toggleBookmarkDisplayAction;
    KAction *m_setHomeAction;
    KAction *m_manageBookmarksAction;
    // Actions for the status bar
    KAction      *m_showPositionAction;
    KAction      *m_showDateTimeAction;
    KAction      *m_showAltitudeAction;
    KAction      *m_showTileZoomLevelAction;
    KAction      *m_showDownloadProgressAction;

    // Action for the tool bar
    KToggleAction *m_showShadow;
    KToggleAction *m_showSunInZenith;

    RenderPluginModel *m_pluginModel;

    KConfigDialog *m_configDialog;

    QHash<QString, int> m_pluginEnabled;

    QString m_position;
    QString m_clock;
    QString m_tileZoomLevel;
    KUrl m_lastFileOpenPath;

    // Items for the statusbar.
    QLabel       *m_positionLabel;
    QLabel       *m_clockLabel;
    QLabel       *m_distanceLabel;
    QLabel       *m_tileZoomLevelLabel;
    QProgressBar *m_downloadProgressBar;

    KParts::StatusBarExtension *m_statusBarExtension;

    // Information about the graphics system
    GraphicsSystem m_initialGraphicsSystem;
    GraphicsSystem m_previousGraphicsSystem;

    QHash< int, int > m_timezone;
    QMap<int, QString> m_externalEditorMapping;
};

}

#endif
