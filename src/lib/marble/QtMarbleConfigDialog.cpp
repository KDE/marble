//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

// Own
#include "QtMarbleConfigDialog.h"

#include "ui_MarbleCacheSettingsWidget.h"
#include "ui_MarbleViewSettingsWidget.h"
#include "ui_MarbleNavigationSettingsWidget.h"
#include "ui_MarbleTimeSettingsWidget.h"
#include "ui_MarbleCloudSyncSettingsWidget.h"

// Qt
#include <QSettings>
#include <QNetworkProxy>
#include <QApplication>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QDateTime>
#include <QTimer>

// Marble
#include "MarbleGlobal.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarblePluginSettingsWidget.h"
#include "MarbleLocale.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "RenderPlugin.h"
#include "RenderPluginModel.h"
#include "MarbleClock.h"
#include "routing/RoutingProfilesWidget.h"
#include "BookmarkSyncManager.h"
#include "CloudSyncManager.h"

namespace Marble
{

class QtMarbleConfigDialogPrivate
{
 public:
    QtMarbleConfigDialogPrivate( MarbleWidget *marbleWidget, CloudSyncManager *cloudSyncManager )
        : ui_viewSettings(),
          ui_navigationSettings(),
          ui_timeSettings(),
          ui_cacheSettings(),
          w_pluginSettings( 0 ),
          m_cloudSyncStatusLabel( 0 ),
          m_marbleWidget( marbleWidget ),
          m_syncManager( cloudSyncManager ? cloudSyncManager->bookmarkSyncManager() : 0 ),
          m_cloudSyncManager(cloudSyncManager),
          m_pluginModel()
    {
    }

    Ui::MarbleViewSettingsWidget       ui_viewSettings;
    Ui::MarbleNavigationSettingsWidget ui_navigationSettings;
    Ui::MarbleTimeSettingsWidget       ui_timeSettings;
    Ui::MarbleCacheSettingsWidget      ui_cacheSettings;
    Ui::MarbleCloudSyncSettingsWidget  ui_cloudSyncSettings;
    MarblePluginSettingsWidget         *w_pluginSettings;

    QSettings m_settings;

    QLabel *m_cloudSyncStatusLabel;

    MarbleWidget *const m_marbleWidget;
    BookmarkSyncManager *const m_syncManager;
    CloudSyncManager *const m_cloudSyncManager;

    RenderPluginModel m_pluginModel;

    QHash< int, int > m_timezone;            
};

QtMarbleConfigDialog::QtMarbleConfigDialog(MarbleWidget *marbleWidget, CloudSyncManager *cloudSyncManager,
                                           QWidget *parent )
    : QDialog( parent ),
      d( new QtMarbleConfigDialogPrivate( marbleWidget, cloudSyncManager ) )
{
    QTabWidget *tabWidget = new QTabWidget( this );
    QDialogButtonBox *buttons = 
    new QDialogButtonBox( QDialogButtonBox::Ok
                        | QDialogButtonBox::Apply
                        | QDialogButtonBox::Cancel,
                          Qt::Horizontal,
                          this );

    // Connect the signals of the ButtonBox
    // to the corresponding slots of the dialog.
    connect( buttons, SIGNAL(accepted()), this, SLOT(accept()) ); // Ok
    connect( buttons, SIGNAL(rejected()), this, SLOT(reject()) ); // Cancel
    connect( buttons->button( QDialogButtonBox::Apply ),SIGNAL(clicked()),
             this, SLOT(writeSettings()) );                         // Apply
    // If the dialog is accepted. Save the settings.
    connect( this, SIGNAL(accepted()), this, SLOT(writeSettings()) );

    // view page
    QWidget *w_viewSettings = new QWidget( this );

    d->ui_viewSettings.setupUi( w_viewSettings );
    tabWidget->addTab( w_viewSettings, tr( "View" ) );

    d->ui_viewSettings.kcfg_labelLocalization->hide();
    d->ui_viewSettings.label_labelLocalization->hide();

    // navigation page
    QWidget *w_navigationSettings = new QWidget( this );

    d->ui_navigationSettings.setupUi( w_navigationSettings );
    tabWidget->addTab( w_navigationSettings, tr( "Navigation" ) );
    d->ui_navigationSettings.kcfg_dragLocation->hide();
    d->ui_navigationSettings.label_dragLocation->hide();

    // cache page
    QWidget *w_cacheSettings = new QWidget( this );

    d->ui_cacheSettings.setupUi( w_cacheSettings );
    tabWidget->addTab( w_cacheSettings, tr( "Cache and Proxy" ) );
    // Forwarding clear button signals
    connect( d->ui_cacheSettings.button_clearVolatileCache, SIGNAL(clicked()), SIGNAL(clearVolatileCacheClicked()) );
    connect( d->ui_cacheSettings.button_clearPersistentCache, SIGNAL(clicked()), SIGNAL(clearPersistentCacheClicked()) );

    // time page
    QWidget *w_timeSettings = new QWidget( this );
    d->ui_timeSettings.setupUi( w_timeSettings );
    tabWidget->addTab( w_timeSettings, tr( "Date and Time" ) );

    // routing page
    QWidget *w_routingSettings = new RoutingProfilesWidget( marbleWidget->model() );
    tabWidget->addTab( w_routingSettings, tr( "Routing" ) );

    // plugin page
    d->m_pluginModel.setRenderPlugins( d->m_marbleWidget->renderPlugins() );
    d->w_pluginSettings = new MarblePluginSettingsWidget( this );
    d->w_pluginSettings->setModel( &d->m_pluginModel );
    d->w_pluginSettings->setObjectName( "plugin_page" );
    tabWidget->addTab( d->w_pluginSettings, tr( "Plugins" ) );

    // Setting the icons for the plugin dialog.
    d->w_pluginSettings->setAboutIcon(QIcon(QStringLiteral(":/icons/help-about.png")));
    d->w_pluginSettings->setConfigIcon(QIcon(QStringLiteral(":/icons/settings-configure.png")));

    connect( this, SIGNAL(rejected()), &d->m_pluginModel, SLOT(retrievePluginState()) );
    connect( this, SIGNAL(accepted()), &d->m_pluginModel, SLOT(applyPluginState()) );
    
    QWidget *w_cloudSyncSettings = new QWidget( this );
    d->ui_cloudSyncSettings.setupUi( w_cloudSyncSettings );
    tabWidget->addTab( w_cloudSyncSettings, tr( "Synchronization" ) );
    d->ui_cloudSyncSettings.button_syncNow->setEnabled( syncBookmarks() );
    d->m_cloudSyncStatusLabel = d->ui_cloudSyncSettings.cloudSyncStatus;
    connect( d->ui_cloudSyncSettings.button_syncNow, SIGNAL(clicked()), SIGNAL(syncNowClicked()) );
    connect( d->ui_cloudSyncSettings.testLoginButton, SIGNAL(clicked()), this, SLOT(updateCloudSyncCredentials()) );

    if ( d->m_syncManager ) {
        connect(d->m_syncManager, SIGNAL(syncComplete()), this, SLOT(updateLastSync()));
        updateLastSync();
    }
    if ( d->m_cloudSyncManager ) {
        connect( d->m_cloudSyncManager, SIGNAL(statusChanged(QString)),
                 this, SLOT(updateCloudSyncStatus(QString)));
    }

    // Layout
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->addWidget( tabWidget );
    layout->addWidget( buttons );
    
    this->setLayout( layout );

    // When the settings have been changed, write to disk.
    connect( this, SIGNAL(settingsChanged()), this, SLOT(syncSettings()) );

    initializeCustomTimezone();
}

QtMarbleConfigDialog::~QtMarbleConfigDialog()
{
    delete d;
}

void QtMarbleConfigDialog::syncSettings()
{
    d->m_settings.sync();
    
    QNetworkProxy proxy;
    
    // Make sure that no proxy is used for an empty string or the default value: 
    if (proxyUrl().isEmpty() || proxyUrl() == QLatin1String("http://")) {
        proxy.setType( QNetworkProxy::NoProxy );
    } else {
        if ( proxyType() == Marble::Socks5Proxy ) {
            proxy.setType( QNetworkProxy::Socks5Proxy );
        }
        else if ( proxyType() == Marble::HttpProxy ) {
            proxy.setType( QNetworkProxy::HttpProxy );
        }
        else {
            mDebug() << "Unknown proxy type! Using Http Proxy instead.";
            proxy.setType( QNetworkProxy::HttpProxy );
        }
    }
    
    proxy.setHostName( proxyUrl() );
    proxy.setPort( proxyPort() );
    
    if ( proxyAuth() ) {
        proxy.setUser( proxyUser() );
        proxy.setPassword( proxyPass() );
    }
    
    QNetworkProxy::setApplicationProxy(proxy);
}

void QtMarbleConfigDialog::updateCloudSyncCredentials()
{
    if ( d->m_cloudSyncManager ) {
        d->m_cloudSyncManager->setOwncloudCredentials(
                    d->ui_cloudSyncSettings.kcfg_owncloudServer->text(),
                    d->ui_cloudSyncSettings.kcfg_owncloudUsername->text(),
                    d->ui_cloudSyncSettings.kcfg_owncloudPassword->text() );
    }
}

void QtMarbleConfigDialog::disableSyncNow()
{
    if ( !d->m_syncManager ) {
        return;
    }

    d->ui_cloudSyncSettings.button_syncNow->setDisabled(true);

    QTimer *timeoutTimer = new QTimer(this);
    connect(timeoutTimer, SIGNAL(timeout()),
            timeoutTimer, SLOT(stop()));
    connect(timeoutTimer, SIGNAL(timeout()),
            this, SLOT(enableSyncNow()));

    connect(d->m_syncManager, SIGNAL(syncComplete()),
            this, SLOT(enableSyncNow()));
    connect(d->m_syncManager, SIGNAL(syncComplete()),
            timeoutTimer, SLOT(stop()));
    connect(d->m_syncManager, SIGNAL(syncComplete()),
            timeoutTimer, SLOT(deleteLater()));

    timeoutTimer->start(30*1000); // 30 sec
}

void QtMarbleConfigDialog::enableSyncNow()
{
    if ( !d->m_syncManager ) {
        return;
    }

    d->ui_cloudSyncSettings.button_syncNow->setEnabled(true);
}

void QtMarbleConfigDialog::updateLastSync()
{
    if ( !d->m_syncManager ) {
        return;
    }

    if (!d->m_syncManager->lastSync().isValid()) {
        d->ui_cloudSyncSettings.labelLastSync->setText(tr("Never synchronized."));
        return;
    }

    const QString title = tr("Last synchronization: %1")
            .arg(d->m_syncManager->lastSync().toString());

    d->ui_cloudSyncSettings.labelLastSync->setText(title);
}

void QtMarbleConfigDialog::readSettings()
{
    // Sync settings to make sure that we read the current settings.
    syncSettings();
    
    // View
    d->ui_viewSettings.kcfg_distanceUnit->setCurrentIndex( measurementSystem() );
    d->ui_viewSettings.kcfg_angleUnit->setCurrentIndex( angleUnit() );
    d->ui_viewSettings.kcfg_stillQuality->setCurrentIndex( stillQuality() );
    d->ui_viewSettings.kcfg_animationQuality->setCurrentIndex( animationQuality() );
    d->ui_viewSettings.kcfg_labelLocalization->setCurrentIndex( Marble::Native );
    d->ui_viewSettings.kcfg_mapFont->setCurrentFont( mapFont() );
    
    // Navigation
    d->ui_navigationSettings.kcfg_dragLocation->setCurrentIndex( Marble::KeepAxisVertically );
    d->ui_navigationSettings.kcfg_onStartup->setCurrentIndex( onStartup() );
    d->ui_navigationSettings.kcfg_inertialEarthRotation->setChecked( inertialEarthRotation() );
    d->ui_navigationSettings.kcfg_animateTargetVoyage->setChecked( animateTargetVoyage() );
    int editorIndex = 0;
    if (externalMapEditor() == QLatin1String("potlatch")) {
        editorIndex = 1;
    } else if (externalMapEditor() == QLatin1String("josm")) {
        editorIndex = 2;
    } else if (externalMapEditor() == QLatin1String("merkaartor")) {
        editorIndex = 3;
    }
    d->ui_navigationSettings.kcfg_externalMapEditor->setCurrentIndex( editorIndex );

    // Cache
    d->ui_cacheSettings.kcfg_volatileTileCacheLimit->setValue( volatileTileCacheLimit() );
    d->ui_cacheSettings.kcfg_persistentTileCacheLimit->setValue( persistentTileCacheLimit() );
    d->ui_cacheSettings.kcfg_proxyUrl->setText( proxyUrl() );
    d->ui_cacheSettings.kcfg_proxyPort->setValue( proxyPort() );
    d->ui_cacheSettings.kcfg_proxyUser->setText( proxyUser() );
    d->ui_cacheSettings.kcfg_proxyPass->setText( proxyPass() );
    d->ui_cacheSettings.kcfg_proxyType->setCurrentIndex( proxyType() );
    d->ui_cacheSettings.kcfg_proxyAuth->setChecked( proxyAuth() );
 
    // Time
    d->ui_timeSettings.kcfg_systemTimezone->setChecked( systemTimezone() );
    d->ui_timeSettings.kcfg_customTimezone->setChecked( customTimezone() );
    d->ui_timeSettings.kcfg_chosenTimezone->setCurrentIndex( chosenTimezone() );
    d->ui_timeSettings.kcfg_utc->setChecked( UTC() );
    d->ui_timeSettings.kcfg_systemTime->setChecked( systemTime() );
    d->ui_timeSettings.kcfg_lastSessionTime->setChecked( lastSessionTime() );
    if( systemTimezone() == true  )
    {
        QDateTime localTime = QDateTime::currentDateTime().toLocalTime();
        localTime.setTimeSpec( Qt::UTC );
        d->m_marbleWidget->model()->setClockTimezone( QDateTime::currentDateTime().toUTC().secsTo( localTime ) );
    }
    else if( UTC() == true )
    {
        d->m_marbleWidget->model()->setClockTimezone( 0 );
    }
    else if( customTimezone() == true )
    {
        d->m_marbleWidget->model()->setClockTimezone( d->m_timezone.value( chosenTimezone() ) );
    }

    // Routing

    // ownCloud
    d->ui_cloudSyncSettings.kcfg_enableSync->setChecked( syncEnabled() );
    d->ui_cloudSyncSettings.kcfg_syncBookmarks->setChecked( syncBookmarks() );
    d->ui_cloudSyncSettings.kcfg_syncRoutes->setChecked( syncRoutes() );
    d->ui_cloudSyncSettings.kcfg_owncloudServer->setText( owncloudServer() );
    d->ui_cloudSyncSettings.kcfg_owncloudUsername->setText( owncloudUsername() );
    d->ui_cloudSyncSettings.kcfg_owncloudPassword->setText( owncloudPassword() );
    
    // Read the settings of the plugins
    d->m_marbleWidget->readPluginSettings( d->m_settings );

    // The settings loaded in the config dialog have been changed.
    emit settingsChanged();
}

void QtMarbleConfigDialog::updateCloudSyncStatus( const QString &status )
{
    d->m_cloudSyncStatusLabel->setText(status);
    CloudSyncManager::Status status_type =
            d->m_cloudSyncManager ? d->m_cloudSyncManager->status() : CloudSyncManager::Unknown;
    switch (status_type) {
        case CloudSyncManager::Success:
            d->m_cloudSyncStatusLabel->setStyleSheet("QLabel { color : green; }");
            break;
        case CloudSyncManager::Error:
            d->m_cloudSyncStatusLabel->setStyleSheet("QLabel { color : red; }");
            break;
        case CloudSyncManager::Unknown:
            d->m_cloudSyncStatusLabel->setStyleSheet("QLabel { color : grey; }");
            break;
    }
}

void QtMarbleConfigDialog::writeSettings()
{
    syncSettings();

    d->m_settings.beginGroup( "View" );
    d->m_settings.setValue( "distanceUnit", d->ui_viewSettings.kcfg_distanceUnit->currentIndex() );
    d->m_settings.setValue( "angleUnit", d->ui_viewSettings.kcfg_angleUnit->currentIndex() );
    d->m_settings.setValue( "stillQuality", d->ui_viewSettings.kcfg_stillQuality->currentIndex() );
    d->m_settings.setValue( "animationQuality", d->ui_viewSettings.kcfg_animationQuality->currentIndex() );
    d->m_settings.setValue( "mapFont", d->ui_viewSettings.kcfg_mapFont->currentFont() );
    d->m_settings.endGroup();
    
    d->m_settings.beginGroup( "Navigation" );
    d->m_settings.setValue( "onStartup", d->ui_navigationSettings.kcfg_onStartup->currentIndex() );
    d->m_settings.setValue( "inertialEarthRotation", d->ui_navigationSettings.kcfg_inertialEarthRotation->isChecked() );
    d->m_settings.setValue( "animateTargetVoyage", d->ui_navigationSettings.kcfg_animateTargetVoyage->isChecked() );
    if( d->ui_navigationSettings.kcfg_externalMapEditor->currentIndex() == 0 ) {
        d->m_settings.setValue( "externalMapEditor", "" );
    } else if( d->ui_navigationSettings.kcfg_externalMapEditor->currentIndex() == 1 ) {
        d->m_settings.setValue( "externalMapEditor", "potlatch" );
    } else if( d->ui_navigationSettings.kcfg_externalMapEditor->currentIndex() == 2 ) {
        d->m_settings.setValue( "externalMapEditor", "josm" );
    } else if( d->ui_navigationSettings.kcfg_externalMapEditor->currentIndex() == 3 ) {
        d->m_settings.setValue( "externalMapEditor", "merkaartor" );
    } else {
        Q_ASSERT( false && "Unexpected index of the external editor setting" );
    }
    d->m_settings.endGroup();
    
    d->m_settings.beginGroup( "Cache" );
    d->m_settings.setValue( "volatileTileCacheLimit", d->ui_cacheSettings.kcfg_volatileTileCacheLimit->value() );
    d->m_settings.setValue( "persistentTileCacheLimit", d->ui_cacheSettings.kcfg_persistentTileCacheLimit->value() );
    d->m_settings.setValue( "proxyUrl", d->ui_cacheSettings.kcfg_proxyUrl->text() );
    d->m_settings.setValue( "proxyPort", d->ui_cacheSettings.kcfg_proxyPort->value() );
    d->m_settings.setValue( "proxyType", d->ui_cacheSettings.kcfg_proxyType->currentIndex() );
    if ( d->ui_cacheSettings.kcfg_proxyAuth->isChecked() ) {
        d->m_settings.setValue( "proxyAuth", true );
        d->m_settings.setValue( "proxyUser", d->ui_cacheSettings.kcfg_proxyUser->text() );
        d->m_settings.setValue( "proxyPass", d->ui_cacheSettings.kcfg_proxyPass->text() );
    } else {
        d->m_settings.setValue( "proxyAuth", false );
    }
    d->m_settings.endGroup();

    d->m_settings.beginGroup( "Time" );
    d->m_settings.setValue( "systemTimezone", d->ui_timeSettings.kcfg_systemTimezone->isChecked() );
    d->m_settings.setValue( "UTC", d->ui_timeSettings.kcfg_utc->isChecked() );
    d->m_settings.setValue( "customTimezone", d->ui_timeSettings.kcfg_customTimezone->isChecked() );
    d->m_settings.setValue( "systemTime", d->ui_timeSettings.kcfg_systemTime->isChecked() );
    d->m_settings.setValue( "lastSessionTime", d->ui_timeSettings.kcfg_lastSessionTime->isChecked() );
    d->m_settings.setValue( "chosenTimezone", d->ui_timeSettings.kcfg_chosenTimezone->currentIndex() );
    d->m_settings.endGroup();
    
    d->m_settings.beginGroup( "CloudSync" );
    d->m_settings.setValue( "enableSync", d->ui_cloudSyncSettings.kcfg_enableSync->isChecked() );
    d->m_settings.setValue( "syncBackend", "owncloud" );
    d->m_settings.setValue( "syncBookmarks", d->ui_cloudSyncSettings.kcfg_syncBookmarks->isChecked() );
    d->m_settings.setValue( "syncRoutes", d->ui_cloudSyncSettings.kcfg_syncRoutes->isChecked() );
    d->m_settings.setValue( "owncloudServer", d->ui_cloudSyncSettings.kcfg_owncloudServer->text() );
    d->m_settings.setValue( "owncloudUsername", d->ui_cloudSyncSettings.kcfg_owncloudUsername->text() );
    d->m_settings.setValue( "owncloudPassword", d->ui_cloudSyncSettings.kcfg_owncloudPassword->text() );
    d->m_settings.endGroup();

    // Plugins
    d->m_marbleWidget->writePluginSettings( d->m_settings );

    emit settingsChanged();
}

MarbleLocale::MeasurementSystem QtMarbleConfigDialog::measurementSystem() const
{
    if( d->m_settings.contains( "View/distanceUnit" ) ) {
        return (MarbleLocale::MeasurementSystem)d->m_settings.value( "View/distanceUnit" ).toInt();
    }

    MarbleLocale *locale = MarbleGlobal::getInstance()->locale();
    return locale->measurementSystem();
}

Marble::AngleUnit QtMarbleConfigDialog::angleUnit() const
{
    return (Marble::AngleUnit) d->m_settings.value( "View/angleUnit", Marble::DMSDegree ).toInt();
}

void QtMarbleConfigDialog::setAngleUnit(Marble::AngleUnit unit)
{
    d->m_settings.setValue( "View/angleUnit", (int)unit );
    d->ui_viewSettings.kcfg_angleUnit->setCurrentIndex( angleUnit() );
    emit settingsChanged();
}

Marble::MapQuality QtMarbleConfigDialog::stillQuality() const
{
    return (Marble::MapQuality) d->m_settings.value( "View/stillQuality",
                                Marble::HighQuality ).toInt();
}

Marble::MapQuality QtMarbleConfigDialog::animationQuality() const
{
    return (Marble::MapQuality) d->m_settings.value( "View/animationQuality",
                                Marble::LowQuality ).toInt();
}

QFont QtMarbleConfigDialog::mapFont() const
{
    return d->m_settings.value( "View/mapFont", QApplication::font() ).value<QFont>();
}

int QtMarbleConfigDialog::onStartup() const
{
    bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    int defaultValue = smallScreen ? Marble::LastLocationVisited : Marble::ShowHomeLocation;
    return d->m_settings.value( "Navigation/onStartup", defaultValue ).toInt();
}

QString QtMarbleConfigDialog::externalMapEditor() const
{
    return d->m_settings.value( "Navigation/externalMapEditor", "" ).toString();
}

bool QtMarbleConfigDialog::animateTargetVoyage() const
{
    const bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    return d->m_settings.value( "Navigation/animateTargetVoyage", smallScreen ).toBool();
}

bool QtMarbleConfigDialog::inertialEarthRotation() const
{
    return d->m_settings.value( "Navigation/inertialEarthRotation", true ).toBool();
}

int QtMarbleConfigDialog::volatileTileCacheLimit() const
{
    int defaultValue = (MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen) ? 6 : 100;
    return d->m_settings.value( "Cache/volatileTileCacheLimit", defaultValue ).toInt();
}

int QtMarbleConfigDialog::persistentTileCacheLimit() const
{
    return d->m_settings.value( "Cache/persistentTileCacheLimit", 0 ).toInt(); // default to unlimited
}

QString QtMarbleConfigDialog::proxyUrl() const
{
    return d->m_settings.value( "Cache/proxyUrl", "" ).toString();
}

int QtMarbleConfigDialog::proxyPort() const
{
    return d->m_settings.value( "Cache/proxyPort", 8080 ).toInt();
}

QString QtMarbleConfigDialog::proxyUser() const
{
    return d->m_settings.value( "Cache/proxyUser", "" ).toString();
}

QString QtMarbleConfigDialog::proxyPass() const
{
    return d->m_settings.value( "Cache/proxyPass", "" ).toString();
}

bool QtMarbleConfigDialog::proxyType() const
{
    return d->m_settings.value( "Cache/proxyType", Marble::HttpProxy ).toInt();
}

bool QtMarbleConfigDialog::proxyAuth() const
{
    return d->m_settings.value( "Cache/proxyAuth", false ).toBool();
}

bool QtMarbleConfigDialog::systemTimezone() const
{
    return d->m_settings.value( "Time/systemTimezone", true ).toBool();
}

bool QtMarbleConfigDialog::customTimezone() const
{
    return d->m_settings.value( "Time/customTimezone", false ).toBool();
}

bool QtMarbleConfigDialog::UTC() const
{
    return d->m_settings.value( "Time/UTC", false ).toBool();
}

bool QtMarbleConfigDialog::systemTime() const
{
    return d->m_settings.value( "Time/systemTime", true ).toBool();
}

bool QtMarbleConfigDialog::lastSessionTime() const
{
    return d->m_settings.value( "Time/lastSessionTime", false ).toBool();
}
    
int QtMarbleConfigDialog::chosenTimezone() const
{
    return d->m_settings.value( "Time/chosenTimezone", 0 ).toInt();
}

void QtMarbleConfigDialog::initializeCustomTimezone()
{
    if( d->m_timezone.count() == 0)
    {
        d->m_timezone.insert( 0, 0 );
        d->m_timezone.insert( 1, 3600 );
        d->m_timezone.insert( 2, 7200 );
        d->m_timezone.insert( 3, 7200 );
        d->m_timezone.insert( 4, 10800 );
        d->m_timezone.insert( 5, 12600 );
        d->m_timezone.insert( 6, 14400 );
        d->m_timezone.insert( 7, 18000 );
        d->m_timezone.insert( 8, 19800 );
        d->m_timezone.insert( 9, 21600 );
        d->m_timezone.insert( 10, 25200 );
        d->m_timezone.insert( 11, 28800 );
        d->m_timezone.insert( 12, 32400 );
        d->m_timezone.insert( 13, 34200 );
        d->m_timezone.insert( 14, 36000 );
        d->m_timezone.insert( 15, 39600 );
        d->m_timezone.insert( 16, 43200 );
        d->m_timezone.insert( 17, -39600 );
        d->m_timezone.insert( 18, -36000 );
        d->m_timezone.insert( 19, -32400 );
        d->m_timezone.insert( 20, -28800 );
        d->m_timezone.insert( 21, -25200 );
        d->m_timezone.insert( 22, -25200 );
        d->m_timezone.insert( 23, -21600 );
        d->m_timezone.insert( 24, -18000 );
        d->m_timezone.insert( 25, -18000 );
        d->m_timezone.insert( 26, -14400 );
        d->m_timezone.insert( 27, -12600 );
        d->m_timezone.insert( 28, -10800 );
        d->m_timezone.insert( 29, -10800 );
        d->m_timezone.insert( 30, -3600 );
    }
}

bool QtMarbleConfigDialog::syncEnabled() const
{
    return d->m_settings.value( "CloudSync/enableSync", false ).toBool();
}

QString QtMarbleConfigDialog::syncBackend() const
{
    return d->m_settings.value( "CloudSync/syncBackend", "" ).toString();
}

bool QtMarbleConfigDialog::syncBookmarks() const
{
    return d->m_settings.value( "CloudSync/syncBookmarks", true ).toBool();
}

bool QtMarbleConfigDialog::syncRoutes() const
{
    return d->m_settings.value( "CloudSync/syncRoutes", true ).toBool();
}

QString QtMarbleConfigDialog::owncloudServer() const
{
    return d->m_settings.value( "CloudSync/owncloudServer", "" ).toString();
}

QString QtMarbleConfigDialog::owncloudUsername() const
{
    return d->m_settings.value( "CloudSync/owncloudUsername", "" ).toString();
}

QString QtMarbleConfigDialog::owncloudPassword() const
{
    return d->m_settings.value( "CloudSync/owncloudPassword", "" ).toString();
}

}

#include "moc_QtMarbleConfigDialog.cpp"
