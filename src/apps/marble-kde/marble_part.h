// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Tobias Koenig <tokoe@kde.org>
// SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>
// SPDX-FileCopyrightText: 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef MARBLE_MARBLEPART_H
#define MARBLE_MARBLEPART_H

#include "MarbleGlobal.h"
#include <KParts/ReadOnlyPart>
#include <KRecentFilesAction>
#include <QDomNode>
#include <QHash>

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

class MarblePart : public KParts::ReadOnlyPart
{
    Q_OBJECT

public:
    MarblePart(QWidget *parentWidget, QObject *parent, const KPluginMetaData &metaData, const QVariantList &);
    ~MarblePart() override;

    ControlView *controlView() const;

    static KAboutData *createAboutData();
    void createInfoBoxesMenu();
    void createOnlineServicesMenu();
    void createRenderPluginActions();

    void readTrackingSettings();

    void initializeCustomTimezone();

public Q_SLOTS:
    bool openUrl(const QUrl &url) override;
    bool openFile() override;
    void showPosition(const QString &position);
    void showZoomLevel(const int);
    void showDateTime();
    void mapThemeChanged(const QString &newMapTheme);
    void createPluginMenus();
    void createFolderList();
    void fallBackToDefaultTheme();

private Q_SLOTS:
    void exportMapScreenShot();
    void printMapScreenShot();
    void copyMap();
    void copyCoordinates();
    void setShowClouds(bool);
    void setShowBookmarks(bool isChecked);
    void showFullScreen(bool);
    void showStatusBar(bool);
    void changeRecordingState();

    /**
     * @brief Show the dateTime label in the status bar.
     *        This slot is connected with the "triggered" signal of
     *        m_showDateTimeAction.
     */
    void showDateTimeLabel(bool isChecked);

    /**
     * @brief Show the position label in the status bar.
     *        This slot is connected with the "triggered" signal of
     *        m_showPositionAction.
     */
    void showPositionLabel(bool isChecked);

    /**
     * @brief Show the altitude label in the status bar.
     *        This slot is connected with the "triggered" signal of
     *        m_showAltitudeAction.
     */
    void showAltitudeLabel(bool isChecked);

    /**
     * @brief Show the tile zoom level label in the status bar.
     *        This slot is connected with the "triggered" signal of
     *        m_showTileZoomLevelAction.
     */
    void showTileZoomLevelLabel(bool isChecked);

    /**
     * @brief Show the download progress bar in the status bar.
     *        This slot is connected with the "triggered" signal of
     *        m_showDownloadProgressAction.
     */
    void showDownloadProgressBar(bool isChecked);

    void handleProgress(int active, int queued);
    void removeProgressItem();

    void lockFloatItemPosition(bool);
    void controlSun();
    void controlTime();
    void showSun(bool);
    void lockToSubSolarPoint(bool);
    void setSubSolarPointIconVisible(bool);
    void workOffline(bool);

    void setupStatusBar();
    void showNewStuffDialog();
    void showDownloadRegionDialog();
    void downloadRegion();
    void showStatusBarContextMenu(const QPoint &pos);
    void showMapWizard();

    void editSettings();

    void enableApplyButton();
    void applyPluginState();

    void updateSettings();

    void updateStatusBar();

    void updateCloudSyncStatus(const QString &status);

    /**
     * Saves the settings of all plugins.
     */
    void writePluginSettings();

    /**
     * Reads the settings of all plugins.
     */
    void readPluginSettings();

    // Bookmark Menu
    void openEditBookmarkDialog();
    void setHome();
    void openManageBookmarksDialog();
    void createBookmarksListMenu(QMenu *, const GeoDataFolder &);
    void lookAtBookmark(QAction *);
    //    void  createBookmarkMenu();

    void updateMapEditButtonVisibility(const QString &mapTheme);
    void showMovieCaptureDialog();
    void stopRecording();
    void updateCloudSyncCredentials();

private:
    void setupActions();
    void setupDownloadProgressBar();
    void setupStatusBarActions();
    QLabel *setupStatusBarLabel(const QString &templateString);

    void readSettings();
    void readStatusBarSettings();
    void writeSettings();
    void writeStatusBarSettings();

    /** Only updates member variable m_tileZoomLevel, does not trigger screen
        update. */
    void updateTileZoomLevel();
    static void migrateNewstuffConfigFiles();
    static void repairNode(QDomNode node, const QString &child);

private:
    // All the functionality is provided by this widget.
    ControlView *m_controlView = nullptr;
    SunControlWidget *m_sunControlDialog = nullptr;
    TimeControlWidget *m_timeControlDialog = nullptr;
    DownloadRegionDialog *m_downloadRegionDialog = nullptr;
    MovieCaptureDialog *m_movieCaptureDialog = nullptr;

    // Actions for the GUI.
    QAction *m_exportMapAction = nullptr;
    QAction *m_printMapAction = nullptr;
    QAction *m_printPreviewAction = nullptr;
    QAction *m_workOfflineAction = nullptr;
    QAction *m_copyMapAction = nullptr;
    QAction *m_copyCoordinatesAction = nullptr;
    QAction *m_showCloudsAction = nullptr;
    QAction *m_fullScreenAct = nullptr;
    QAction *m_openAct = nullptr;
    QAction *m_newStuffAction = nullptr;
    QAction *m_downloadRegionAction = nullptr;
    QAction *m_controlSunAction = nullptr;
    QAction *m_controlTimeAction = nullptr;
    QAction *m_lockFloatItemsAct = nullptr;
    QAction *m_mapWizardAct = nullptr;
    QAction *m_externalMapEditorAction = nullptr;
    QAction *m_recordMovieAction = nullptr;
    QAction *m_stopRecordingAction = nullptr;
    KRecentFilesAction *m_recentFilesAction = nullptr;

    // Bookmark Menu
    QAction *m_addBookmarkAction = nullptr;
    QAction *m_toggleBookmarkDisplayAction = nullptr;
    QAction *m_setHomeAction = nullptr;
    QAction *m_manageBookmarksAction = nullptr;
    // Actions for the status bar
    QAction *m_showPositionAction = nullptr;
    QAction *m_showDateTimeAction = nullptr;
    QAction *m_showAltitudeAction = nullptr;
    QAction *m_showTileZoomLevelAction = nullptr;
    QAction *m_showDownloadProgressAction = nullptr;

    // Action for the tool bar
    KToggleAction *m_showShadow = nullptr;
    KToggleAction *m_lockToSubSolarPoint = nullptr;
    KToggleAction *m_setSubSolarPointIconVisible = nullptr;

    KConfigDialog *m_configDialog = nullptr;

    QHash<QString, int> m_pluginEnabled;

    QString m_position;
    QString m_clock;
    QString m_tileZoomLevel;
    QString m_lastFileOpenPath;

    // Items for the statusbar.
    QLabel *m_positionLabel = nullptr;
    QLabel *m_clockLabel = nullptr;
    QLabel *m_distanceLabel = nullptr;
    QLabel *m_tileZoomLevelLabel = nullptr;
    QProgressBar *m_downloadProgressBar = nullptr;

    KParts::StatusBarExtension *m_statusBarExtension = nullptr;

    QHash<int, int> m_timezone;
    QMap<int, QString> m_externalEditorMapping;
    Ui_MarbleCloudSyncSettingsWidget m_ui_cloudSyncSettings;
};

}

#endif
