// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_QTMARBLECONFIGDIALOG_H
#define MARBLE_QTMARBLECONFIGDIALOG_H

#include <QDialog>

#include "marble_export.h"
#include "MarbleGlobal.h"
#include "MarbleLocale.h"

namespace Marble
{

class MarbleWidget;
class CloudSyncManager;

class QtMarbleConfigDialogPrivate;

class MARBLE_EXPORT QtMarbleConfigDialog : public QDialog
{
    Q_OBJECT
    
    public:
    explicit QtMarbleConfigDialog(MarbleWidget *marbleWidget, CloudSyncManager *syncManager = nullptr,
                                   QWidget *parent = nullptr );
    ~QtMarbleConfigDialog() override;

    // View Settings
    
    MarbleLocale::MeasurementSystem measurementSystem() const;
    Marble::AngleUnit angleUnit() const;
    void setAngleUnit(Marble::AngleUnit unit);
    Marble::MapQuality stillQuality() const;
    Marble::MapQuality animationQuality() const;
    QFont mapFont() const;

    // Navigation Settings
    int onStartup() const;
    bool animateTargetVoyage() const;
    QString externalMapEditor() const;
    bool inertialEarthRotation() const;
    bool mouseViewRotation() const;

    // Cache Settings
    int volatileTileCacheLimit() const;
    int persistentTileCacheLimit() const;
    QString proxyUrl() const;
    int proxyPort() const;

    QString proxyUser() const;
    QString proxyPass() const;
    bool proxyType() const;
    bool proxyAuth() const;

    // Time Settings
    /**
     * Read the value of 'Time/systemTime' key from settings
     */
    bool systemTime() const;

    /**
     * Read the value of 'Time/lastSessionTime' key from settings
     */
    bool lastSessionTime() const;

    /**
     * Read the value of 'Time/systemTimezone' key from settings
     */
    bool systemTimezone() const;

    /**
     * Read the value of 'Time/UTC' key from settings
     */
    bool UTC() const;

    /**
     * Read the value of 'Time/customTimezone' key from settings
     */
    bool customTimezone() const;

    /**
     * Read the value of 'Time/chosenTimezone' key from settings
     */
    int chosenTimezone() const;
    
    void initializeCustomTimezone();
    
    // CloudSync settings
    bool syncEnabled() const;
    QString syncBackend() const;
    bool syncBookmarks() const;
    bool syncRoutes() const;
    QString owncloudServer() const;
    QString owncloudUsername() const;
    QString owncloudPassword() const;

    Q_SIGNALS:
    /**
     * This signal is emitted when the loaded settings were changed.
     * Either by the user or by loading them initially from disk.
     */
    void settingsChanged();

    /**
     * The user clicked on the button to clear volatile tile cache.
     */
    void clearVolatileCacheClicked();

    /**
     * The user clicked on the button to clear persistent tile cache.
     */
    void clearPersistentCacheClicked();

    /**
     * The user clicked on the button to manually synchronize bookmarks.
     */
    void syncNowClicked();

    public Q_SLOTS:
    /**
     * Disable "Sync Now" button while sync or for 30 sec of timout
     */
    void disableSyncNow();
    void enableSyncNow();

    /**
     * Sets new title for the "Last Sync" label indicator
     */
    void updateLastSync();

    /**
     * Read settings and update interface.
     */
    void readSettings();

    /**
     * Show status on cloud sync settings tab
     */
    void updateCloudSyncStatus( const QString &status );

    /**
     * Write settings to disk.
     */
    void writeSettings();
    
    private Q_SLOTS:
    /**
     * Synchronize the loaded settings with the file on hard disk.
     */
    void syncSettings();

    void updateCloudSyncCredentials();

    private:
    Q_DISABLE_COPY( QtMarbleConfigDialog )

    QtMarbleConfigDialogPrivate * const d;
};

} // Marble namespace

#endif
