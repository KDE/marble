//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef QTMARBLECONFIGDIALOG_H
#define QTMARBLECONFIGDIALOG_H

#include <QtGui/QDialog>
#include <lib/global.h>

#include "lib/ui_MarbleViewSettingsWidget.h"
#include "lib/ui_MarbleNavigationSettingsWidget.h"

class QSettings;
class QStandardItemModel;

namespace Marble {

class ControlView;
class MarbleCacheSettingsWidget;
class MarblePluginSettingsWidget;

class QtMarbleConfigDialog : public QDialog
{
    Q_OBJECT
    
    public:
    QtMarbleConfigDialog( ControlView *controlView = 0, QWidget *parent = 0 );
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
    bool proxyHttp() const;
    bool proxySocks5() const;
    bool proxyAuth() const;

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

    private:
    Q_DISABLE_COPY( QtMarbleConfigDialog )

    void initSettings();

    Ui::MarbleViewSettingsWidget       ui_viewSettings;
    Ui::MarbleNavigationSettingsWidget ui_navigationSettings;
    MarbleCacheSettingsWidget          *w_cacheSettings;
    MarblePluginSettingsWidget         *w_pluginSettings;

    QSettings *settings;
    
    ControlView *m_controlView;
    
    QStandardItemModel* m_pluginModel;

    // Information about the graphics system
    Marble::GraphicsSystem m_initialGraphicsSystem;
    Marble::GraphicsSystem m_previousGraphicsSystem;
};

} // Marble namespace

#endif // QTMARBLECONFIGDIALOG
