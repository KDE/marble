//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_QTMARBLECONFIGDIALOG_H
#define MARBLE_QTMARBLECONFIGDIALOG_H

#include <QtGui/QDialog>

#include "marble_export.h"
#include <global.h>

#include "ui_MarbleViewSettingsWidget.h"
#include "ui_MarbleNavigationSettingsWidget.h"
#include "ui_MarbleTimeSettingsWidget.h"


namespace Marble
{

class MarbleWidget;

class QtMarbleConfigDialogPrivate;

class MARBLE_EXPORT QtMarbleConfigDialog : public QDialog
{
    Q_OBJECT
    
    public:
    explicit QtMarbleConfigDialog( MarbleWidget *marbleWidget, QWidget *parent = 0 );
    ~QtMarbleConfigDialog();

    // View Settings
    
    Marble::DistanceUnit distanceUnit() const;
    Marble::AngleUnit angleUnit() const;
    Marble::MapQuality stillQuality() const;
    Marble::MapQuality animationQuality() const;
    int labelLocalization() const;
    QFont mapFont() const;

    // View
    Marble::GraphicsSystem graphicsSystem() const;


    // Navigation Settings
    int dragLocation() const;
    int onStartup() const;
    bool animateTargetVoyage() const;

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

    Q_SIGNALS:
    /**
     * This signal is emitted when when the loaded settings were changed.
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

    public Q_SLOTS:
    /**
     * Read settings and update interface.
     */
    void readSettings();

    /**
     * Write settings to disk.
     */
    void writeSettings();
    
    private Q_SLOTS:
    /**
     * Retrieve the current plugin state for the user interface.
     */
    void retrievePluginState();
    
    /**
     * Apply the plugin state from the user interface.
     */
    void applyPluginState();
    
    /**
     * Synchronize the loaded settings with the file on hard disk.
     */
    void syncSettings();

    /**
     * Shows the about dialog for the plugin with the corresponding @p nameId.
     */
    void showPluginAboutDialog( QString nameId );

    /**
     * Shows the configuration dialog for the plugin with the corresponding @p nameId.
     */
    void showPluginConfigDialog( QString nameId );

    /**
     * Saves the settings of all plugins.
     */
    void writePluginSettings();

    private:
    Q_DISABLE_COPY( QtMarbleConfigDialog )

    QtMarbleConfigDialogPrivate * const d;
};

} // Marble namespace

#endif
