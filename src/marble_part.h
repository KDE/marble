//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig  <tokoe@kde.org>
// Copyright 2008      Inge Wallin    <inge@lysator.liu.se>
// Copyright 2009      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#ifndef MARBLE_MARBLEPART_H
#define MARBLE_MARBLEPART_H

#include <kparts/part.h>
#include <QtCore/QHash>
#include <kmenu.h>
#include "global.h"

class KAboutData;
class KAction;
class KConfigDialog;

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
class SunControlWidget;
class GeoDataFolder;

class MarblePart: public KParts::ReadOnlyPart
{
  Q_OBJECT

  public:
    MarblePart( QWidget *parentWidget, QObject *parent, const QStringList& );
    virtual ~MarblePart();

    ControlView *controlView() const;

    static KAboutData* createAboutData();
    void  createInfoBoxesMenu();
    void createOnlineServicesMenu();
	void  createFolderList();
  public Q_SLOTS:
    bool  openUrl( const KUrl &url );
    bool  openFile();
    void  showPosition( const QString& position);
    void  showDistance( const QString& position);
    void  showZoomLevel( const int );
    void  mapThemeChanged( const QString& newMapTheme );
    void  createPluginMenus();

  private Q_SLOTS:
    void  initObject();

    void  exportMapScreenShot();
    void  printMapScreenShot();
    void  printPixmap( QPrinter * printer, const QPixmap& pixmap );
    void  printPreview();
    void  paintPrintPreview( QPrinter * printer );
    void  copyMap();
    void  copyCoordinates();
    void  setShowClouds( bool );
    void  setShowAtmosphere( bool );
    void  setShowCurrentLocation( bool );
    void  showFullScreen( bool );
    void  showSideBar( bool );
    void  showStatusBar( bool );

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
    void  showSun( bool );
    void  workOffline( bool );

    void  setupStatusBar();
    void  showNewStuffDialog();
    void  connectDownloadRegionDialog();
    void  disconnectDownloadRegionDialog();
    void  showDownloadRegionDialog();
    void  downloadRegion();
    void  showStatusBarContextMenu( const QPoint& pos );

    void  editSettings();

    void  enableApplyButton();
    void  applyPluginState();
    void  retrievePluginState();

    void  updateSettings();
    void reload();

    /**
     * Shows the about dialog for the plugin with the corresponding @p nameId.
     */
    void  showPluginAboutDialog( const QString& nameId );

    /**
     * Shows the configuration dialog for the plugin with the corresponding @p nameId.
     */
    void  showPluginConfigDialog( const QString& nameId );

    /**
     * Saves the settings of all plugins.
     */
    void writePluginSettings();

    /**
     * Reads the settings of all plugins.
     */
    void readPluginSettings();

    //Bookmark Menu
    void  openBookmarkInfoDialog();
    void  createBookmarksListMenu( QMenu *, const GeoDataFolder& );
    void  lookAtBookmark( QAction * );
    void  removeAllBookmarks();
    void  openNewBookmarkFolderDialog();
//    void  createBookmarkMenu();


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
    DownloadRegionDialog *m_downloadRegionDialog;

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
    KAction      *m_lockFloatItemsAct;

    //Bookmark Menu
    KAction *m_addBookmarkAction;
    KAction *m_removeAllBookmarksAction;
    KAction *m_addBookmarkFolderAction;
    // Actions for the status bar
    KAction      *m_showPositionAction;
    KAction      *m_showAltitudeAction;
    KAction      *m_showTileZoomLevelAction;
    KAction      *m_showDownloadProgressAction;

    QStandardItemModel* m_pluginModel;

    KConfigDialog *m_configDialog;

    QHash<QString, int> m_pluginEnabled;

    QString m_position;
    QString m_distance;
    QString m_tileZoomLevel;

    // Items for the statusbar.
    QLabel       *m_positionLabel;
    QLabel       *m_distanceLabel;
    QLabel       *m_tileZoomLevelLabel;
    QProgressBar *m_downloadProgressBar;

    void updateStatusBar();

    KParts::StatusBarExtension *m_statusBarExtension;

    // Information about the graphics system
    GraphicsSystem m_initialGraphicsSystem;
    GraphicsSystem m_previousGraphicsSystem;
};

}

#endif
