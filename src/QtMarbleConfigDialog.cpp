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
#include <QtCore/QDebug>
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

// Marble
#include "lib/global.h"
#include "ControlView.h"
#include "lib/MarbleCacheSettingsWidget.h"
#include "lib/MarblePluginSettingsWidget.h"
#include "lib/MarbleWidget.h"
#include "lib/RenderPlugin.h"

using namespace Marble;

QtMarbleConfigDialog::QtMarbleConfigDialog( ControlView *controlView, QWidget *parent )
    : QDialog( parent ),
      ui_viewSettings(),
      ui_navigationSettings(),
      m_controlView( controlView )
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

    ui_viewSettings.setupUi( w_viewSettings );
    tabWidget->addTab( w_viewSettings, tr( "View" ) );

    // It's experimental -- so we remove it for now.
    // FIXME: Delete the following  line once OpenGL support is officially supported.
    ui_viewSettings.kcfg_graphicsSystem->removeItem( Marble::OpenGLGraphics );

    QString nativeString ( tr("Native") );

    #ifdef Q_WS_X11
    nativeString = tr( "Native (X11)" );
    #endif
    #ifdef Q_WS_MAC
    nativeString = tr( "Native (Mac OS X Core Graphics)" );
    #endif

    ui_viewSettings.kcfg_graphicsSystem->setItemText( Marble::NativeGraphics, nativeString );

    // navigation page
    QWidget *w_navigationSettings = new QWidget( this );

    ui_navigationSettings.setupUi( w_navigationSettings );
    tabWidget->addTab( w_navigationSettings, tr( "Navigation" ) );

    // cache page
    w_cacheSettings = new MarbleCacheSettingsWidget( this );
    tabWidget->addTab( w_cacheSettings, tr( "Cache and Proxy" ) );
    // Forwarding clear button signals
    connect( w_cacheSettings,               SIGNAL( clearVolatileCache() ),
             this, SIGNAL( clearVolatileCacheClicked() ) );
    connect( w_cacheSettings,               SIGNAL( clearPersistentCache() ),
             this, SIGNAL( clearPersistentCacheClicked() ) );
    
    // plugin page
    m_pluginModel = new QStandardItemModel( this );
    QStandardItem  *parentItem = m_pluginModel->invisibleRootItem();

    QList<RenderPlugin *>  pluginList = m_controlView->marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    for (; i != pluginList.constEnd(); ++i) {
        parentItem->appendRow( (*i)->item() );
    }

    MarblePluginSettingsWidget *w_pluginSettings = new MarblePluginSettingsWidget( this );
    w_pluginSettings->setModel( m_pluginModel );
    w_pluginSettings->setObjectName( "plugin_page" );
    tabWidget->addTab( w_pluginSettings, tr( "Plugins" ) );

    connect( w_pluginSettings, SIGNAL( pluginListViewClicked() ),
                               SLOT( slotEnableButtonApply() ) );
    connect( w_pluginSettings, SIGNAL( aboutPluginClicked( QString ) ),
                               SLOT( showPluginAboutDialog( QString ) ) );
    connect( w_pluginSettings, SIGNAL( configPluginClicked( QString ) ),
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
    
    initSettings();
}

QtMarbleConfigDialog::~QtMarbleConfigDialog()
{
    delete settings;
}

void QtMarbleConfigDialog::initSettings() 
{
    settings = new QSettings("kde.org", "Marble Desktop Globe");
}

void QtMarbleConfigDialog::syncSettings()
{
    settings->sync();
    
    QNetworkProxy proxy;
    
    // Make sure that no proxy is used for an empty string or the default value: 
    if ( proxyUrl().isEmpty() || proxyUrl() == "http://" ) {
        proxy.setType( QNetworkProxy::NoProxy );
    } else if ( proxyHttp() ) {
        proxy.setType( QNetworkProxy::HttpProxy );
    } else if ( proxySocks5() ) {
        proxy.setType( QNetworkProxy::Socks5Proxy );
    }
    
    proxy.setHostName( proxyUrl() );
    proxy.setPort( proxyPort() );
    
    if ( proxyAuth() ) {
        proxy.setUser( proxyUser() );
        proxy.setPassword( proxyPass() );
    }
    
    QNetworkProxy::setApplicationProxy(proxy);
}

void QtMarbleConfigDialog::showPluginAboutDialog( QString nameId ) {
    QList<RenderPlugin *> renderItemList = m_controlView->marbleWidget()->renderPlugins();

    foreach ( RenderPlugin *renderItem, renderItemList ) {
        if( renderItem->nameId() == nameId ) {
            QDialog *aboutDialog = renderItem->aboutDialog();
            if ( aboutDialog ) {
                aboutDialog->show();
            }
        }
    }
}

void QtMarbleConfigDialog::showPluginConfigDialog( QString nameId ) {
    QList<RenderPlugin *> renderItemList = m_controlView->marbleWidget()->renderPlugins();

    foreach ( RenderPlugin *renderItem, renderItemList ) {
        if( renderItem->nameId() == nameId ) {
            QDialog *configDialog = renderItem->configDialog();
            if ( configDialog ) {
                configDialog->show();
            }
        }
    }
}


void QtMarbleConfigDialog::readSettings()
{
    m_initialGraphicsSystem = graphicsSystem();
    m_previousGraphicsSystem = m_initialGraphicsSystem;

    // Sync settings to make sure that we read the current settings.
    syncSettings();
    
    // View
    ui_viewSettings.kcfg_distanceUnit->setCurrentIndex( distanceUnit() );
    ui_viewSettings.kcfg_angleUnit->setCurrentIndex( angleUnit() );
    ui_viewSettings.kcfg_stillQuality->setCurrentIndex( stillQuality() );
    ui_viewSettings.kcfg_animationQuality->setCurrentIndex( animationQuality() );
    ui_viewSettings.kcfg_labelLocalization->setCurrentIndex( labelLocalization() );
    ui_viewSettings.kcfg_mapFont->setCurrentFont( mapFont() );
    ui_viewSettings.kcfg_graphicsSystem->setCurrentIndex( graphicsSystem() );
    
    // Navigation
    ui_navigationSettings.kcfg_dragLocation->setCurrentIndex( dragLocation() );
    ui_navigationSettings.kcfg_onStartup->setCurrentIndex( onStartup() );
    if( animateTargetVoyage() )
    ui_navigationSettings.kcfg_animateTargetVoyage->setCheckState( Qt::Checked );
    else
    ui_navigationSettings.kcfg_animateTargetVoyage->setCheckState( Qt::Unchecked );
    
    // Cache
    w_cacheSettings->kcfg_volatileTileCacheLimit->setValue( volatileTileCacheLimit() );
    w_cacheSettings->kcfg_persistentTileCacheLimit->setValue( persistentTileCacheLimit() );
    w_cacheSettings->kcfg_proxyUrl->setText( proxyUrl() );
    w_cacheSettings->kcfg_proxyPort->setValue( proxyPort() );
    w_cacheSettings->kcfg_proxyUser->setText( proxyUser() );
    w_cacheSettings->kcfg_proxyPass->setText( proxyPass() );
    w_cacheSettings->kcfg_proxyHttp->setChecked( proxyHttp() );
    w_cacheSettings->kcfg_proxySocks5->setChecked( proxySocks5() );
    if ( proxyAuth() ) {
        w_cacheSettings->kcfg_proxyAuth->setCheckState( Qt::Checked );
    } else {
        w_cacheSettings->kcfg_proxyAuth->setCheckState( Qt::Unchecked );
    }
    
    // Plugins
    QList<QVariant> pluginNameIdList;
    QList<QVariant> pluginEnabledList;
    QList<QVariant> pluginVisibleList;
    
    settings->beginGroup( "Plugins" );
        pluginNameIdList = settings->value( "pluginNameId" ).toList();
        pluginEnabledList = settings->value( "pluginEnabled" ).toList();
        pluginVisibleList = settings->value( "pluginVisible" ).toList();
    settings->endGroup();
        
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

    QList<RenderPlugin *> pluginList = m_controlView->marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    for (; i != pluginList.constEnd(); ++i) {
        if ( pluginEnabled.contains( (*i)->nameId() ) ) {
            (*i)->setEnabled( pluginEnabled[ (*i)->nameId() ] );
        }
        if ( pluginVisible.contains( (*i)->nameId() ) ) {
            (*i)->setVisible( pluginVisible[ (*i)->nameId() ] );
        }
    }
    
    // The settings loaded in the config dialog have been changed.
    emit settingsChanged();
}

void QtMarbleConfigDialog::writeSettings()
{
    syncSettings();

    // Determining the graphicsSystemString
    QString graphicsSystemString;
    switch ( ui_viewSettings.kcfg_graphicsSystem->currentIndex() )
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
    
    settings->beginGroup( "View" );
    settings->setValue( "distanceUnit", ui_viewSettings.kcfg_distanceUnit->currentIndex() );
    settings->setValue( "angleUnit", ui_viewSettings.kcfg_angleUnit->currentIndex() );
    settings->setValue( "stillQuality", ui_viewSettings.kcfg_stillQuality->currentIndex() );
    settings->setValue( "animationQuality", ui_viewSettings.kcfg_animationQuality->currentIndex() );
    settings->setValue( "labelLocalization", ui_viewSettings.kcfg_labelLocalization->currentIndex() );
    settings->setValue( "mapFont", ui_viewSettings.kcfg_mapFont->currentFont() );
    settings->setValue( "graphicsSystem", graphicsSystemString );
    settings->endGroup();
    
    settings->beginGroup( "Navigation" );
    settings->setValue( "dragLocation", ui_navigationSettings.kcfg_dragLocation->currentIndex() );
    settings->setValue( "onStartup", ui_navigationSettings.kcfg_onStartup->currentIndex() );
    if( ui_navigationSettings.kcfg_animateTargetVoyage->checkState() == Qt::Checked )
        settings->setValue( "animateTargetVoyage", true );
    else
        settings->setValue( "animateTargetVoyage", false );
    settings->endGroup();
    
    settings->beginGroup( "Cache" );
    settings->setValue( "volatileTileCacheLimit", w_cacheSettings->kcfg_volatileTileCacheLimit->value() );
    settings->setValue( "persistentTileCacheLimit", w_cacheSettings->kcfg_persistentTileCacheLimit->value() );
    settings->setValue( "proxyUrl", w_cacheSettings->kcfg_proxyUrl->text() );
    settings->setValue( "proxyPort", w_cacheSettings->kcfg_proxyPort->value() );
    settings->setValue( "proxyHttp", w_cacheSettings->kcfg_proxyHttp->isChecked() );
    settings->setValue( "proxySocks5", w_cacheSettings->kcfg_proxySocks5->isChecked() );
    if ( w_cacheSettings->kcfg_proxyAuth->isChecked() ) {
        settings->setValue( "proxyAuth", true );
        settings->setValue( "proxyUser", w_cacheSettings->kcfg_proxyUser->text() );
        settings->setValue( "proxyPass", w_cacheSettings->kcfg_proxyPass->text() );
    } else {
        settings->setValue( "proxyAuth", false );
    }
    settings->endGroup();
    
    // Plugins
    QList<QVariant>   pluginEnabled;
    QList<QVariant>   pluginVisible;
    QStringList  pluginNameId;
 
    QList<RenderPlugin *> pluginList = m_controlView->marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    for (; i != pluginList.constEnd(); ++i) {
        pluginEnabled << static_cast<int>( (*i)->enabled() );
        pluginVisible << static_cast<int>( (*i)->visible() );
        pluginNameId  << (*i)->nameId();
    }
    
    settings->beginGroup( "Plugins" );
        settings->setValue( "pluginNameId", pluginNameId );
        settings->setValue( "pluginEnabled", pluginEnabled );
        settings->setValue( "pluginVisible", pluginVisible );
    settings->endGroup();
    
    emit settingsChanged();

    if (    m_initialGraphicsSystem != graphicsSystem() 
         && m_previousGraphicsSystem != graphicsSystem() ) {
        QMessageBox::information (this, tr("Graphics System Change"),
                                tr("You have decided to run Marble with a different graphics system.\n"
                                   "For this change to become effective, Marble has to be restarted.\n"
                                   "Please close the application and start Marble again.") );
    }    
    m_previousGraphicsSystem = graphicsSystem();
}

void QtMarbleConfigDialog::retrievePluginState() {
    QList<RenderPlugin *>  pluginList = m_controlView->marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    for (; i != pluginList.constEnd(); ++i) {
        (*i)->retrieveItemState();
    }
}

void QtMarbleConfigDialog::applyPluginState() {
    QList<RenderPlugin *>  pluginList = m_controlView->marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    for (; i != pluginList.constEnd(); ++i) {
        (*i)->applyItemState();
    }
}

Marble::DistanceUnit QtMarbleConfigDialog::distanceUnit() const
{
    return (Marble::DistanceUnit) settings->value( "View/distanceUnit", Marble::Metric ).toInt();
}

Marble::AngleUnit QtMarbleConfigDialog::angleUnit() const
{
    return (Marble::AngleUnit) settings->value( "View/angleUnit", Marble::DMSDegree ).toInt();
}

Marble::MapQuality QtMarbleConfigDialog::stillQuality() const
{
    return (Marble::MapQuality) settings->value( "View/stillQuality", Marble::High ).toInt();
}

Marble::MapQuality QtMarbleConfigDialog::animationQuality() const
{
    return (Marble::MapQuality) settings->value( "View/animationQuality", Marble::Low ).toInt();
}

int QtMarbleConfigDialog::labelLocalization() const
{
    return settings->value( "View/labelLocalization" , Marble::Native ).toInt();
}

QFont QtMarbleConfigDialog::mapFont() const
{
    return settings->value( "View/mapFont", QApplication::font() ).value<QFont>();
}

Marble::GraphicsSystem QtMarbleConfigDialog::graphicsSystem() const
{
    QString graphicsSystemString = settings->value( "View/graphicsSystem", "native" ).toString();

    if ( graphicsSystemString == "raster" ) return Marble::RasterGraphics;
    if ( graphicsSystemString == "opengl" ) return Marble::OpenGLGraphics;

    // default case:  graphicsSystemString == "native"
    return Marble::NativeGraphics;
}

int QtMarbleConfigDialog::dragLocation() const
{
    return settings->value( "Navigation/dragLocation", Marble::KeepAxisVertically ).toInt();
}

int QtMarbleConfigDialog::onStartup() const
{
    return settings->value( "Navigation/onStartup", Marble::ShowHomeLocation ).toInt();
}

bool QtMarbleConfigDialog::animateTargetVoyage() const
{
    return settings->value( "Navigation/animateTargetVoyage", false ).toBool();
}

int QtMarbleConfigDialog::volatileTileCacheLimit() const
{
    return settings->value( "Cache/volatileTileCacheLimit", 30 ).toInt();
}

int QtMarbleConfigDialog::persistentTileCacheLimit() const
{
    return settings->value( "Cache/persistentTileCacheLimit", 300 ).toInt();
}

QString QtMarbleConfigDialog::proxyUrl() const
{
    return settings->value( "Cache/proxyUrl", "http://" ).toString();
}

int QtMarbleConfigDialog::proxyPort() const
{
    return settings->value( "Cache/proxyPort", 8080 ).toInt();
}

QString QtMarbleConfigDialog::proxyUser() const
{
    return settings->value( "Cache/proxyUser", "" ).toString();
}

QString QtMarbleConfigDialog::proxyPass() const
{
    return settings->value( "Cache/proxyPass", "" ).toString();
}

bool QtMarbleConfigDialog::proxyHttp() const
{
    return settings->value( "Cache/proxyHttp", "" ).toBool();
}

bool QtMarbleConfigDialog::proxySocks5() const
{
    return settings->value( "Cache/proxySocks5", "" ).toBool();
}

bool QtMarbleConfigDialog::proxyAuth() const
{
    return settings->value( "Cache/proxyAuth", false ).toBool();
}

#include "QtMarbleConfigDialog.moc"
