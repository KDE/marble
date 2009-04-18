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
    
class MarbleCacheSettingsWidget;

class QtMarbleConfigDialog : public QDialog
{
    Q_OBJECT
    
    public:
    QtMarbleConfigDialog( QWidget *parent = 0 );
    ~QtMarbleConfigDialog();

    // View Settings
    Marble::DistanceUnit distanceUnit();
    Marble::AngleUnit angleUnit();
    Marble::MapQuality stillQuality();
    Marble::MapQuality animationQuality();
    int labelLocalization();
    QFont mapFont();

    // Navigation Settings
    int dragLocation();
    int onStartup();
    bool animateTargetVoyage();

    // Cache Settings
    int volatileTileCacheLimit();
    int persistentTileCacheLimit();
    QString proxyUrl();
    int proxyPort();

    QString proxyUser();
    QString proxyPass();
    bool proxyHttp();
    bool proxySocks5();
    bool proxyAuth();

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

    private Q_SLOTS:
    /**
     * Write settings to disk.
     */
    void writeSettings();

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
};

} // Marble namespace

#endif // QTMARBLECONFIGDIALOG
