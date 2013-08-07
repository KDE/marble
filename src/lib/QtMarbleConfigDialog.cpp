//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
// Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

// Own
#include "QtMarbleConfigDialog.h"

// Qt
#include <QList>
#include <QSettings>
#include <QNetworkProxy>
#include <QApplication>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardItem>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QDateTime>

// Marble
#include "MarbleGlobal.h"
#include "DialogConfigurationInterface.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleCacheSettingsWidget.h"
#include "MarbleNavigationSettingsWidget.h"
#include "MarblePluginSettingsWidget.h"
#include "MarbleTimeSettingsWidget.h"
#include "MarbleViewSettingsWidget.h"
#include "MarbleLocale.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "RenderPlugin.h"
#include "RenderPluginModel.h"
#include "MarbleClock.h"
#include "routing/RoutingProfilesWidget.h"

namespace Marble
{

class QtMarbleConfigDialogPrivate
{
 public:
    QtMarbleConfigDialogPrivate( MarbleWidget *marbleWidget )
        : w_viewSettings( 0 ),
          w_navigationSettings( 0 ),
          w_timeSettings( 0 ),
          w_cacheSettings( 0 ),
          w_pluginSettings( 0 ),
          m_marbleWidget( marbleWidget ),
          m_pluginModel()
    {
    }

    MarbleViewSettingsWidget           *w_viewSettings;
    MarbleNavigationSettingsWidget     *w_navigationSettings;
    MarbleTimeSettingsWidget           *w_timeSettings;
    MarbleCacheSettingsWidget          *w_cacheSettings;
    MarblePluginSettingsWidget         *w_pluginSettings;

    QSettings m_settings;

    MarbleWidget *const m_marbleWidget;

    RenderPluginModel m_pluginModel;

    QHash< int, int > m_timezone;            

    // Information about the graphics system
    Marble::GraphicsSystem m_initialGraphicsSystem;
    Marble::GraphicsSystem m_previousGraphicsSystem;
};

QtMarbleConfigDialog::QtMarbleConfigDialog( MarbleWidget *marbleWidget, QWidget *parent )
    : QDialog( parent ),
      d( new QtMarbleConfigDialogPrivate( marbleWidget ) )
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
    d->w_viewSettings = new MarbleViewSettingsWidget( this );
    tabWidget->addTab( d->w_viewSettings, tr( "View" ) );

    // navigation page
    d->w_navigationSettings = new MarbleNavigationSettingsWidget( this );
    tabWidget->addTab( d->w_navigationSettings, tr( "Navigation" ) );

    // cache page
    d->w_cacheSettings = new MarbleCacheSettingsWidget( this );
    tabWidget->addTab( d->w_cacheSettings, tr( "Cache and Proxy" ) );
    // Forwarding clear button signals
    connect( d->w_cacheSettings, SIGNAL(clearVolatileCache()),
             this,               SIGNAL(clearVolatileCacheClicked()) );
    connect( d->w_cacheSettings, SIGNAL(clearPersistentCache()),
             this,               SIGNAL(clearPersistentCacheClicked()) );

    // time page
    d->w_timeSettings = new MarbleTimeSettingsWidget( this );
    tabWidget->addTab( d->w_timeSettings, tr( "Date and Time" ) );

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
    d->w_pluginSettings->setAboutIcon( QIcon(":/icons/help-about.png") );
    d->w_pluginSettings->setConfigIcon(  QIcon(":/icons/settings-configure.png") );

    connect( this, SIGNAL(rejected()), &d->m_pluginModel, SLOT(retrievePluginState()) );
    connect( this, SIGNAL(accepted()), &d->m_pluginModel, SLOT(applyPluginState()) );

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
    if ( proxyUrl().isEmpty() || proxyUrl() == "http://" ) {
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

void QtMarbleConfigDialog::readSettings()
{
    d->m_initialGraphicsSystem = graphicsSystem();
    d->m_previousGraphicsSystem = d->m_initialGraphicsSystem;

    // Sync settings to make sure that we read the current settings.
    syncSettings();

    // View
    d->w_viewSettings->setMeasurementSystem( measurementSystem() );
    d->w_viewSettings->setAngleUnit( angleUnit() );
    d->w_viewSettings->setStillQuality( stillQuality() );
    d->w_viewSettings->setAnimationQuality( animationQuality() );
    d->w_viewSettings->setLabelLocalization( Marble::Native );
    d->w_viewSettings->setMapFont( mapFont() );
    d->w_viewSettings->setGraphicsSystem( graphicsSystem() );

    // Navigation
    d->w_navigationSettings->setDragLocation( Marble::KeepAxisVertically );
    d->w_navigationSettings->setStartupLocation( onStartup() );
    d->w_navigationSettings->setInertialEarthRotationEnabled( inertialEarthRotation() );
    d->w_navigationSettings->setAnimateTargetVoyage( animateTargetVoyage() );
    d->w_navigationSettings->setExternalMapEditor( externalMapEditor() );

    // Cache
    d->w_cacheSettings->setVolatileTileCacheLimit( volatileTileCacheLimit() );
    d->w_cacheSettings->setPersistentTileCacheLimit( persistentTileCacheLimit() );
    d->w_cacheSettings->setProxyUrl( proxyUrl() );
    d->w_cacheSettings->setProxyPort( proxyPort() );
    d->w_cacheSettings->setProxyUser( proxyUser() );
    d->w_cacheSettings->setProxyPassword( proxyPass() );
    d->w_cacheSettings->setProxyType( proxyType() );
    d->w_cacheSettings->setProxyAuthenticationEnabled( proxyAuth() );

    // Time
    d->w_timeSettings->setSystemTimeEnabled( systemTimezone() );
    d->w_timeSettings->setCustomTimezoneEnabled( customTimezone() );
    d->w_timeSettings->setCustomTimezone( chosenTimezone() );
    d->w_timeSettings->setUtcEnabled( UTC() );
    d->w_timeSettings->setSystemTimeEnabled( systemTime() );
    d->w_timeSettings->setLastSessionTimeEnabled( lastSessionTime() );
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

    // Read the settings of the plugins
    d->m_marbleWidget->readPluginSettings( d->m_settings );

    // The settings loaded in the config dialog have been changed.
    emit settingsChanged();
}

void QtMarbleConfigDialog::writeSettings()
{
    syncSettings();

    // Determining the graphicsSystemString
    QString graphicsSystemString;
    switch ( d->w_viewSettings->graphicsSystem() )
    {
        case Marble::RasterGraphics :
            graphicsSystemString = "raster";
            break;
        case Marble::OpenGLGraphics :
            graphicsSystemString = "opengl";
            break;
        default:
        case Marble::NativeGraphics :
            graphicsSystemString = "native";
            break;
    }
    
    d->m_settings.beginGroup( "View" );
    d->m_settings.setValue( "distanceUnit", d->w_viewSettings->measurementSystem() );
    d->m_settings.setValue( "angleUnit", d->w_viewSettings->angleUnit() );
    d->m_settings.setValue( "stillQuality", d->w_viewSettings->stillQuality() );
    d->m_settings.setValue( "animationQuality", d->w_viewSettings->animationQuality() );
    d->m_settings.setValue( "mapFont", d->w_viewSettings->mapFont() );
    d->m_settings.setValue( "graphicsSystem", graphicsSystemString );
    d->m_settings.endGroup();
    
    d->m_settings.beginGroup( "Navigation" );
    d->m_settings.setValue( "onStartup", d->w_navigationSettings->startupLocation() );
    d->m_settings.setValue( "inertialEarthRotation", d->w_navigationSettings->isInertialEarthRotationEnabled() );
    d->m_settings.setValue( "animateTargetVoyage", d->w_navigationSettings->isTargetVoyageAnimationEnabled() );
    d->m_settings.setValue( "externalMapEditor", d->w_navigationSettings->externalMapEditor() );
    d->m_settings.endGroup();
    
    d->m_settings.beginGroup( "Cache" );
    d->m_settings.setValue( "volatileTileCacheLimit", d->w_cacheSettings->volatileTileCacheLimit() );
    d->m_settings.setValue( "persistentTileCacheLimit", d->w_cacheSettings->persistentTileCacheLimit() );
    d->m_settings.setValue( "proxyUrl", d->w_cacheSettings->proxyUrl() );
    d->m_settings.setValue( "proxyPort", d->w_cacheSettings->proxyPort() );
    d->m_settings.setValue( "proxyType", d->w_cacheSettings->proxyType() );
    if ( d->w_cacheSettings->isProxyAuthenticationEnabled() ) {
        d->m_settings.setValue( "proxyAuth", true );
        d->m_settings.setValue( "proxyUser", d->w_cacheSettings->proxyUser() );
        d->m_settings.setValue( "proxyPass", d->w_cacheSettings->proxyPassword() );
    } else {
        d->m_settings.setValue( "proxyAuth", false );
    }
    d->m_settings.endGroup();

    d->m_settings.beginGroup( "Time" );
    d->m_settings.setValue( "systemTimezone", d->w_timeSettings->isSystemTimezoneEnabled() );
    d->m_settings.setValue( "UTC", d->w_timeSettings->isUtcEnabled() );
    d->m_settings.setValue( "customTimezone", d->w_timeSettings->isCustomTimezoneEnabled() );
    d->m_settings.setValue( "systemTime", d->w_timeSettings->isSystemTimeEnabled() );
    d->m_settings.setValue( "lastSessionTime", d->w_timeSettings->isLastSessionTimeEnabled() );
    d->m_settings.setValue( "chosenTimezone", d->w_timeSettings->customTimezone() );
    d->m_settings.endGroup();

    // Plugins
    d->m_marbleWidget->writePluginSettings( d->m_settings );

    emit settingsChanged();

    if (    d->m_initialGraphicsSystem != graphicsSystem()
         && d->m_previousGraphicsSystem != graphicsSystem() ) {
        QMessageBox::information (this, tr("Graphics System Change"),
                                tr("You have decided to run Marble with a different graphics system.\n"
                                   "For this change to become effective, Marble has to be restarted.\n"
                                   "Please close the application and start Marble again.") );
    }    
    d->m_previousGraphicsSystem = graphicsSystem();
}

QLocale::MeasurementSystem QtMarbleConfigDialog::measurementSystem() const
{
    if( d->m_settings.contains( "View/distanceUnit" ) ) {
        return (QLocale::MeasurementSystem) d->m_settings.value( "View/distanceUnit" ).toInt();
    }

    MarbleLocale *locale = MarbleGlobal::getInstance()->locale();

    return locale->measurementSystem();
}

Marble::AngleUnit QtMarbleConfigDialog::angleUnit() const
{
    return (Marble::AngleUnit) d->m_settings.value( "View/angleUnit", Marble::DMSDegree ).toInt();
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

Marble::GraphicsSystem QtMarbleConfigDialog::graphicsSystem() const
{
    QString graphicsSystemString = d->m_settings.value( "View/graphicsSystem", "raster" ).toString();

    if ( graphicsSystemString == "raster" ) return Marble::RasterGraphics;
    if ( graphicsSystemString == "opengl" ) return Marble::OpenGLGraphics;

    // default case:  graphicsSystemString == "raster"
    return Marble::NativeGraphics;
}

OnStartup QtMarbleConfigDialog::onStartup() const
{
    bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    int defaultValue = smallScreen ? Marble::LastLocationVisited : Marble::ShowHomeLocation;
    return (Marble::OnStartup) d->m_settings.value( "Navigation/onStartup", defaultValue ).toInt();
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
    int defaultValue = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ? 6 : 100;
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

quint16 QtMarbleConfigDialog::proxyPort() const
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

ProxyType QtMarbleConfigDialog::proxyType() const
{
    return static_cast<ProxyType>( d->m_settings.value( "Cache/proxyType", Marble::HttpProxy ).toInt() );
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


}

#include "QtMarbleConfigDialog.moc"
