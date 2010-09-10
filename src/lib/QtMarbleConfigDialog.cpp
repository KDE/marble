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

// Marble
#include "global.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleCacheSettingsWidget.h"
#include "MarblePluginSettingsWidget.h"
#include "MarbleLocale.h"
#include "MarbleWidget.h"
#include "RenderPlugin.h"

namespace Marble
{

class QtMarbleConfigDialogPrivate
{
 public:
    QtMarbleConfigDialogPrivate( MarbleWidget *marbleWidget )
        : ui_viewSettings(),
          ui_navigationSettings(),
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
    MarbleCacheSettingsWidget          *w_cacheSettings;
    MarblePluginSettingsWidget         *w_pluginSettings;

    QSettings *m_settings;

    MarbleWidget *m_marbleWidget;

    QStandardItemModel* m_pluginModel;

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
    return d->m_settings->value( "Navigation/onStartup", Marble::ShowHomeLocation ).toInt();
}

bool QtMarbleConfigDialog::animateTargetVoyage() const
{
    return d->m_settings->value( "Navigation/animateTargetVoyage", false ).toBool();
}

int QtMarbleConfigDialog::volatileTileCacheLimit() const
{
    return d->m_settings->value( "Cache/volatileTileCacheLimit", 30 ).toInt();
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

}

#include "QtMarbleConfigDialog.moc"
