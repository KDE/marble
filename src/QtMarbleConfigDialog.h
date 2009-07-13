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

#include "lib/MarbleCacheSettingsWidget.h"

class QSettings;

namespace Marble {
    
class ControlView;
class MarbleCacheSettingsWidget;

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
    ProxyType proxyType() const;
    bool proxyAuth() const;

    Q_SIGNALS:
    /**
     * This signal is emitted when the user changed settings and
     * chose to Apply.
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
     * Synchronize the loaded settings with the file on hard disk.
     */
    void syncSettings();

    private:
    Q_DISABLE_COPY( QtMarbleConfigDialog )

    void initSettings();

    Ui::MarbleViewSettingsWidget       ui_viewSettings;
    Ui::MarbleNavigationSettingsWidget ui_navigationSettings;
    MarbleCacheSettingsWidget          *w_cacheSettings;

    QSettings *settings;
    
    ControlView *m_controlView;

    // Information about the graphics system
    Marble::GraphicsSystem m_initialGraphicsSystem;
    Marble::GraphicsSystem m_previousGraphicsSystem;
};

} // Marble namespace

#endif // QTMARBLECONFIGDIALOG
