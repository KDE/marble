//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Own
#include "QtMarbleConfigDialog.h"

// Qt
#include <QtCore/QList>
#include <QtCore/QSettings>
#include <QtNetwork/QNetworkProxy>
#include <QtGui/QApplication>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QMessageBox>
#include <QtGui/QStandardItem>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtCore/QDateTime>

// Marble
#include "global.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleCacheSettingsWidget.h"
#include "MarblePluginSettingsWidget.h"
#include "MarbleLocale.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "RenderPlugin.h"
#include "MarbleClock.h"
#include "routing/RoutingProfilesWidget.h"

namespace Marble
{

class QtMarbleConfigDialogPrivate
{
 public:
    QtMarbleConfigDialogPrivate( MarbleWidget *marbleWidget )
        : ui_viewSettings(),
          ui_navigationSettings(),
          ui_timeSettings(),
          m_marbleWidget( marbleWidget )
    {
    }

    ~QtMarbleConfigDialogPrivate()
    {
        delete m_settings;
    }

    void initSettings()
    {
        m_settings = new QSettings("kde.org", "Marble Desktop Globe");
    }

    Ui::MarbleViewSettingsWidget       ui_viewSettings;
    Ui::MarbleNavigationSettingsWidget ui_navigationSettings;
    Ui::MarbleTimeSettingsWidget       ui_timeSettings;
    MarbleCacheSettingsWidget          *w_cacheSettings;
    MarblePluginSettingsWidget         *w_pluginSettings;

    QSettings *m_settings;

    MarbleWidget *m_marbleWidget;

    QStandardItemModel* m_pluginModel;

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
    connect( buttons, SIGNAL( accepted() ), this, SLOT( accept() ) ); // Ok
    connect( buttons, SIGNAL( rejected() ), this, SLOT( reject() ) ); // Cancel
    connect( buttons->button( QDialogButtonBox::Apply ),SIGNAL( clicked() ),
             this, SLOT( writeSettings() ) );                         // Apply
    // If the dialog is accepted. Save the settings.
    connect( this, SIGNAL( accepted() ), this, SLOT( writeSettings() ) );

    // view page
    QWidget *w_viewSettings = new QWidget( this );

    d->ui_viewSettings.setupUi( w_viewSettings );
    tabWidget->addTab( w_viewSettings, tr( "View" ) );

    // It's experimental -- so we remove it for now.
    // FIXME: Delete the following  line once OpenGL support is officially supported.
    d->ui_viewSettings.kcfg_graphicsSystem->removeItem( Marble::OpenGLGraphics );

    QString nativeString ( tr("Native") );

    #ifdef Q_WS_X11
    nativeString = tr( "Native (X11)" );
    #endif
    #ifdef Q_WS_MAC
    nativeString = tr( "Native (Mac OS X Core Graphics)" );
    #endif

    d->ui_viewSettings.kcfg_graphicsSystem->setItemText( Marble::NativeGraphics, nativeString );

    // navigation page
    QWidget *w_navigationSettings = new QWidget( this );

    d->ui_navigationSettings.setupUi( w_navigationSettings );
    tabWidget->addTab( w_navigationSettings, tr( "Navigation" ) );

    // cache page
    d->w_cacheSettings = new MarbleCacheSettingsWidget( this );
    tabWidget->addTab( d->w_cacheSettings, tr( "Cache and Proxy" ) );
    // Forwarding clear button signals
    connect( d->w_cacheSettings, SIGNAL( clearVolatileCache() ),
             this,               SIGNAL( clearVolatileCacheClicked() ) );
    connect( d->w_cacheSettings, SIGNAL( clearPersistentCache() ),
             this,               SIGNAL( clearPersistentCacheClicked() ) );

    // time page
    QWidget *w_timeSettings = new QWidget( this );
    d->ui_timeSettings.setupUi( w_timeSettings );
    tabWidget->addTab( w_timeSettings, tr( "Date and Time" ) );

    // routing page
    QWidget *w_routingSettings = new RoutingProfilesWidget( marbleWidget );
    tabWidget->addTab( w_routingSettings, tr( "Routing" ) );

    // plugin page
    d->m_pluginModel = new QStandardItemModel( this );
    QStandardItem  *parentItem = d->m_pluginModel->invisibleRootItem();

    QList<RenderPlugin *>  pluginList = d->m_marbleWidget->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = pluginList.constEnd();
    for (; i != end; ++i ) {
        parentItem->appendRow( (*i)->item() );
    }

    d->w_pluginSettings = new MarblePluginSettingsWidget( this );
    d->w_pluginSettings->setModel( d->m_pluginModel );
    d->w_pluginSettings->setObjectName( "plugin_page" );
    tabWidget->addTab( d->w_pluginSettings, tr( "Plugins" ) );

    // Setting the icons for the plugin dialog.
    d->w_pluginSettings->setAboutIcon( QIcon(":/icons/help-about.png") );
    d->w_pluginSettings->setConfigIcon(  QIcon(":/icons/settings-configure.png") );

    connect( d->w_pluginSettings, SIGNAL( aboutPluginClicked( QString ) ),
                                  SLOT( showPluginAboutDialog( QString ) ) );
    connect( d->w_pluginSettings, SIGNAL( configPluginClicked( QString ) ),
                                  SLOT( showPluginConfigDialog( QString ) ) );
    connect( this, SIGNAL( rejected() ), this, SLOT( retrievePluginState() ) );
    connect( this, SIGNAL( accepted() ), this, SLOT( applyPluginState() ) );

    // Layout
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->addWidget( tabWidget );
    layout->addWidget( buttons );
    
    this->setLayout( layout );

    // When the settings have been changed, write to disk.
    connect( this, SIGNAL( settingsChanged() ), this, SLOT( syncSettings() ) );

    connect( d->m_marbleWidget, SIGNAL( pluginSettingsChanged() ),
             this,              SLOT( writePluginSettings() ) );
    
    d->initSettings();
    initializeCustomTimezone();
}

QtMarbleConfigDialog::~QtMarbleConfigDialog()
{
    delete d;
}

void QtMarbleConfigDialog::syncSettings()
{
    d->m_settings->sync();
    
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

void QtMarbleConfigDialog::showPluginAboutDialog( QString nameId )
{
    QList<RenderPlugin *> renderItemList = d->m_marbleWidget->renderPlugins();

    foreach ( RenderPlugin *renderItem, renderItemList ) {
        if( renderItem->nameId() == nameId ) {
            QDialog *aboutDialog = renderItem->aboutDialog();
            if ( aboutDialog ) {
                aboutDialog->show();
            }
        }
    }
}

void QtMarbleConfigDialog::showPluginConfigDialog( QString nameId )
{
    QList<RenderPlugin *> renderItemList = d->m_marbleWidget->renderPlugins();

    foreach ( RenderPlugin *renderItem, renderItemList ) {
        if( renderItem->nameId() == nameId ) {
            QDialog *configDialog = renderItem->configDialog();
            if ( configDialog ) {
                configDialog->show();
            }
        }
    }
}

void QtMarbleConfigDialog::writePluginSettings()
{
    d->m_marbleWidget->writePluginSettings( *d->m_settings );
}

void QtMarbleConfigDialog::readSettings()
{
    d->m_initialGraphicsSystem = graphicsSystem();
    d->m_previousGraphicsSystem = d->m_initialGraphicsSystem;

    // Sync settings to make sure that we read the current settings.
    syncSettings();
    
    // View
    d->ui_viewSettings.kcfg_distanceUnit->setCurrentIndex( distanceUnit() );
    d->ui_viewSettings.kcfg_angleUnit->setCurrentIndex( angleUnit() );
    d->ui_viewSettings.kcfg_stillQuality->setCurrentIndex( stillQuality() );
    d->ui_viewSettings.kcfg_animationQuality->setCurrentIndex( animationQuality() );
    d->ui_viewSettings.kcfg_labelLocalization->setCurrentIndex( labelLocalization() );
    d->ui_viewSettings.kcfg_mapFont->setCurrentFont( mapFont() );
    d->ui_viewSettings.kcfg_graphicsSystem->setCurrentIndex( graphicsSystem() );
    
    // Navigation
    d->ui_navigationSettings.kcfg_dragLocation->setCurrentIndex( dragLocation() );
    d->ui_navigationSettings.kcfg_onStartup->setCurrentIndex( onStartup() );
    if( animateTargetVoyage() )
    d->ui_navigationSettings.kcfg_animateTargetVoyage->setCheckState( Qt::Checked );
    else
    d->ui_navigationSettings.kcfg_animateTargetVoyage->setCheckState( Qt::Unchecked );
    int editorIndex = externalMapEditor() == "merkaartor" ? 2 : externalMapEditor() == "josm" ? 1 : 0;
    d->ui_navigationSettings.kcfg_externalEditor->setCurrentIndex( editorIndex );

    // Cache
    d->w_cacheSettings->kcfg_volatileTileCacheLimit->setValue( volatileTileCacheLimit() );
    d->w_cacheSettings->kcfg_persistentTileCacheLimit->setValue( persistentTileCacheLimit() );
    d->w_cacheSettings->kcfg_proxyUrl->setText( proxyUrl() );
    d->w_cacheSettings->kcfg_proxyPort->setValue( proxyPort() );
    d->w_cacheSettings->kcfg_proxyUser->setText( proxyUser() );
    d->w_cacheSettings->kcfg_proxyPass->setText( proxyPass() );
    d->w_cacheSettings->kcfg_proxyType->setCurrentIndex( proxyType() );
    if ( proxyAuth() ) {
        d->w_cacheSettings->kcfg_proxyAuth->setCheckState( Qt::Checked );
    } else {
        d->w_cacheSettings->kcfg_proxyAuth->setCheckState( Qt::Unchecked );
    }
 
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

    // Plugins
    QList<QVariant> pluginNameIdList;
    QList<QVariant> pluginEnabledList;
    QList<QVariant> pluginVisibleList;
    
    d->m_settings->beginGroup( "Plugins" );
        pluginNameIdList = d->m_settings->value( "pluginNameId" ).toList();
        pluginEnabledList = d->m_settings->value( "pluginEnabled" ).toList();
        pluginVisibleList = d->m_settings->value( "pluginVisible" ).toList();
    d->m_settings->endGroup();
        
    QHash<QString, int> pluginEnabled;
    QHash<QString, int> pluginVisible;
    
    int nameIdSize = pluginNameIdList.size();
    int enabledSize = pluginEnabledList.size();
    int visibleSize = pluginVisibleList.size();

    if ( nameIdSize == enabledSize ) {
        for ( int i = 0; i < enabledSize; ++i ) {
            pluginEnabled[ pluginNameIdList[i].toString() ]
                = pluginEnabledList[i].toInt();
        }
    }
    
    if ( nameIdSize == visibleSize ) {
        for ( int i = 0; i < visibleSize; ++i ) {
            pluginVisible[ pluginNameIdList[i].toString() ]
                = pluginVisibleList[i].toInt();
        }
    }

    QList<RenderPlugin *> pluginList = d->m_marbleWidget->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = pluginList.constEnd();
    for (; i != end; ++i ) {
        if ( pluginEnabled.contains( (*i)->nameId() ) ) {
            (*i)->setEnabled( pluginEnabled[ (*i)->nameId() ] );
        }
        if ( pluginVisible.contains( (*i)->nameId() ) ) {
            (*i)->setVisible( pluginVisible[ (*i)->nameId() ] );
        }
    }

    // Read the settings of the plugins
    d->m_marbleWidget->readPluginSettings( *d->m_settings );
    
    // The settings loaded in the config dialog have been changed.
    emit settingsChanged();
}

void QtMarbleConfigDialog::writeSettings()
{
    syncSettings();

    // Determining the graphicsSystemString
    QString graphicsSystemString;
    switch ( d->ui_viewSettings.kcfg_graphicsSystem->currentIndex() )
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
    
    d->m_settings->beginGroup( "View" );
    d->m_settings->setValue( "distanceUnit", d->ui_viewSettings.kcfg_distanceUnit->currentIndex() );
    d->m_settings->setValue( "angleUnit", d->ui_viewSettings.kcfg_angleUnit->currentIndex() );
    d->m_settings->setValue( "stillQuality", d->ui_viewSettings.kcfg_stillQuality->currentIndex() );
    d->m_settings->setValue( "animationQuality", d->ui_viewSettings.kcfg_animationQuality->currentIndex() );
    d->m_settings->setValue( "labelLocalization", d->ui_viewSettings.kcfg_labelLocalization->currentIndex() );
    d->m_settings->setValue( "mapFont", d->ui_viewSettings.kcfg_mapFont->currentFont() );
    d->m_settings->setValue( "graphicsSystem", graphicsSystemString );
    d->m_settings->endGroup();
    
    d->m_settings->beginGroup( "Navigation" );
    d->m_settings->setValue( "dragLocation", d->ui_navigationSettings.kcfg_dragLocation->currentIndex() );
    d->m_settings->setValue( "onStartup", d->ui_navigationSettings.kcfg_onStartup->currentIndex() );
    if( d->ui_navigationSettings.kcfg_animateTargetVoyage->checkState() == Qt::Checked )
        d->m_settings->setValue( "animateTargetVoyage", true );
    else
        d->m_settings->setValue( "animateTargetVoyage", false );
    if( d->ui_navigationSettings.kcfg_externalEditor->currentIndex() == 0 ) {
        d->m_settings->setValue( "externalMapEditor", "" );
    } else if( d->ui_navigationSettings.kcfg_externalEditor->currentIndex() == 1 ) {
        d->m_settings->setValue( "externalMapEditor", "josm" );
    } else if( d->ui_navigationSettings.kcfg_externalEditor->currentIndex() == 2 ) {
        d->m_settings->setValue( "externalMapEditor", "merkaartor" );
    } else {
        Q_ASSERT( false && "Unexpected index of the external editor setting" );
    }
    d->m_settings->endGroup();
    
    d->m_settings->beginGroup( "Cache" );
    d->m_settings->setValue( "volatileTileCacheLimit", d->w_cacheSettings->kcfg_volatileTileCacheLimit->value() );
    d->m_settings->setValue( "persistentTileCacheLimit", d->w_cacheSettings->kcfg_persistentTileCacheLimit->value() );
    d->m_settings->setValue( "proxyUrl", d->w_cacheSettings->kcfg_proxyUrl->text() );
    d->m_settings->setValue( "proxyPort", d->w_cacheSettings->kcfg_proxyPort->value() );
    d->m_settings->setValue( "proxyType", d->w_cacheSettings->kcfg_proxyType->currentIndex() );
    if ( d->w_cacheSettings->kcfg_proxyAuth->isChecked() ) {
        d->m_settings->setValue( "proxyAuth", true );
        d->m_settings->setValue( "proxyUser", d->w_cacheSettings->kcfg_proxyUser->text() );
        d->m_settings->setValue( "proxyPass", d->w_cacheSettings->kcfg_proxyPass->text() );
    } else {
        d->m_settings->setValue( "proxyAuth", false );
    }
    d->m_settings->endGroup();

    d->m_settings->beginGroup( "Time" );
    d->m_settings->setValue( "systemTimezone", d->ui_timeSettings.kcfg_systemTimezone->isChecked() );
    d->m_settings->setValue( "UTC", d->ui_timeSettings.kcfg_utc->isChecked() );
    d->m_settings->setValue( "customTimezone", d->ui_timeSettings.kcfg_customTimezone->isChecked() );
    d->m_settings->setValue( "systemTime", d->ui_timeSettings.kcfg_systemTime->isChecked() );
    d->m_settings->setValue( "lastSessionTime", d->ui_timeSettings.kcfg_lastSessionTime->isChecked() );
    d->m_settings->setValue( "chosenTimezone", d->ui_timeSettings.kcfg_chosenTimezone->currentIndex() );
    d->m_settings->endGroup();

    // Plugins
    QList<QVariant>   pluginEnabled;
    QList<QVariant>   pluginVisible;
    QStringList  pluginNameId;
 
    QList<RenderPlugin *> pluginList = d->m_marbleWidget->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = pluginList.constEnd();
    for (; i != end; ++i ) {
        pluginEnabled << static_cast<int>( (*i)->enabled() );
        pluginVisible << static_cast<int>( (*i)->visible() );
        pluginNameId  << (*i)->nameId();
    }
    
    d->m_settings->beginGroup( "Plugins" );
        d->m_settings->setValue( "pluginNameId", pluginNameId );
        d->m_settings->setValue( "pluginEnabled", pluginEnabled );
        d->m_settings->setValue( "pluginVisible", pluginVisible );
    d->m_settings->endGroup();
    
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

void QtMarbleConfigDialog::retrievePluginState()
{
    QList<RenderPlugin *>  pluginList = d->m_marbleWidget->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = pluginList.constEnd();
    for (; i != end; ++i ) {
        (*i)->retrieveItemState();
    }
}

void QtMarbleConfigDialog::applyPluginState()
{
    QList<RenderPlugin *>  pluginList = d->m_marbleWidget->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = pluginList.constEnd();
    for (; i != end; ++i ) {
        (*i)->applyItemState();
    }
}

Marble::DistanceUnit QtMarbleConfigDialog::distanceUnit() const
{
    if( d->m_settings->contains( "View/distanceUnit" ) ) {
        return (Marble::DistanceUnit) d->m_settings->value( "View/distanceUnit" ).toInt();
    }
    else {
        MarbleLocale *locale = MarbleGlobal::getInstance()->locale();
        
        if ( locale->measureSystem() == Marble::Metric ) {
            return Marble::Meter;
        }
        else {
            return Marble::MilesFeet;
        }
    }
}

Marble::AngleUnit QtMarbleConfigDialog::angleUnit() const
{
    return (Marble::AngleUnit) d->m_settings->value( "View/angleUnit", Marble::DMSDegree ).toInt();
}

Marble::MapQuality QtMarbleConfigDialog::stillQuality() const
{
    return (Marble::MapQuality) d->m_settings->value( "View/stillQuality",
                                Marble::HighQuality ).toInt();
}

Marble::MapQuality QtMarbleConfigDialog::animationQuality() const
{
    return (Marble::MapQuality) d->m_settings->value( "View/animationQuality",
                                Marble::LowQuality ).toInt();
}

int QtMarbleConfigDialog::labelLocalization() const
{
    return d->m_settings->value( "View/labelLocalization" , Marble::Native ).toInt();
}

QFont QtMarbleConfigDialog::mapFont() const
{
    return d->m_settings->value( "View/mapFont", QApplication::font() ).value<QFont>();
}

Marble::GraphicsSystem QtMarbleConfigDialog::graphicsSystem() const
{
    QString graphicsSystemString = d->m_settings->value( "View/graphicsSystem", "native" ).toString();

    if ( graphicsSystemString == "raster" ) return Marble::RasterGraphics;
    if ( graphicsSystemString == "opengl" ) return Marble::OpenGLGraphics;

    // default case:  graphicsSystemString == "native"
    return Marble::NativeGraphics;
}

int QtMarbleConfigDialog::dragLocation() const
{
    return d->m_settings->value( "Navigation/dragLocation", Marble::KeepAxisVertically ).toInt();
}

int QtMarbleConfigDialog::onStartup() const
{
    bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    int defaultValue = smallScreen ? Marble::LastLocationVisited : Marble::ShowHomeLocation;
    return d->m_settings->value( "Navigation/onStartup", defaultValue ).toInt();
}

QString QtMarbleConfigDialog::externalMapEditor() const
{
    return d->m_settings->value( "Navigation/externalMapEditor", "" ).toString();
}

bool QtMarbleConfigDialog::animateTargetVoyage() const
{
    return d->m_settings->value( "Navigation/animateTargetVoyage", false ).toBool();
}

int QtMarbleConfigDialog::volatileTileCacheLimit() const
{
    int defaultValue = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ? 6 : 30;
    return d->m_settings->value( "Cache/volatileTileCacheLimit", defaultValue ).toInt();
}

int QtMarbleConfigDialog::persistentTileCacheLimit() const
{
    return d->m_settings->value( "Cache/persistentTileCacheLimit", 300 ).toInt();
}

QString QtMarbleConfigDialog::proxyUrl() const
{
    return d->m_settings->value( "Cache/proxyUrl", "" ).toString();
}

int QtMarbleConfigDialog::proxyPort() const
{
    return d->m_settings->value( "Cache/proxyPort", 8080 ).toInt();
}

QString QtMarbleConfigDialog::proxyUser() const
{
    return d->m_settings->value( "Cache/proxyUser", "" ).toString();
}

QString QtMarbleConfigDialog::proxyPass() const
{
    return d->m_settings->value( "Cache/proxyPass", "" ).toString();
}

bool QtMarbleConfigDialog::proxyType() const
{
    return d->m_settings->value( "Cache/proxyType", Marble::HttpProxy ).toInt();
}

bool QtMarbleConfigDialog::proxyAuth() const
{
    return d->m_settings->value( "Cache/proxyAuth", false ).toBool();
}

bool QtMarbleConfigDialog::systemTimezone() const
{
    return d->m_settings->value( "Time/systemTimezone", true ).toBool();
}

bool QtMarbleConfigDialog::customTimezone() const
{
    return d->m_settings->value( "Time/customTimezone", false ).toBool();
}

bool QtMarbleConfigDialog::UTC() const
{
    return d->m_settings->value( "Time/UTC", false ).toBool();
}

bool QtMarbleConfigDialog::systemTime() const
{
    return d->m_settings->value( "Time/systemTime", true ).toBool();
}

bool QtMarbleConfigDialog::lastSessionTime() const
{
    return d->m_settings->value( "Time/lastSessionTime", false ).toBool();
}
    
int QtMarbleConfigDialog::chosenTimezone() const
{
    return d->m_settings->value( "Time/chosenTimezone", 0 ).toInt();
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
