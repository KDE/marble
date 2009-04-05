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
#include <QtGui/QStandardItem>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

// Marble
#include "lib/MarbleCacheSettingsWidget.h"
#include "lib/global.h"

using namespace Marble;

#include "lib/ui_MarbleViewSettingsWidget.h"
#include "lib/ui_MarbleNavigationSettingsWidget.h"

QtMarbleConfigDialog::QtMarbleConfigDialog( QWidget *parent )
    : QDialog( parent )
{
    QTabWidget *tabWidget = new QTabWidget( this );
    QDialogButtonBox *buttons = 
	new QDialogButtonBox(   QDialogButtonBox::Ok
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
    ui_viewSettings = new Ui::MarbleViewSettingsWidget;
    QWidget *w_viewSettings = new QWidget( 0 );

    ui_viewSettings->setupUi( w_viewSettings );
    tabWidget->addTab( w_viewSettings, tr( "View" ) );

    // navigation page
    ui_navigationSettings = new Ui::MarbleNavigationSettingsWidget;
    QWidget *w_navigationSettings = new QWidget( 0 );

    ui_navigationSettings->setupUi( w_navigationSettings );
    tabWidget->addTab( w_navigationSettings, tr( "Navigation" ) );

    // cache page
    w_cacheSettings = new MarbleCacheSettingsWidget();
    tabWidget->addTab( w_cacheSettings, tr( "Cache and Proxy" ) );
    // Forwarding clear button signals
     connect( w_cacheSettings,               SIGNAL( clearVolatileCache() ),
 	      this, SIGNAL( clearVolatileCacheClicked() ) );
     connect( w_cacheSettings,               SIGNAL( clearPersistentCache() ),
 	      this, SIGNAL( clearPersistentCacheClicked() ) );
    
    QVBoxLayout *layout = new QVBoxLayout;
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
#ifdef Q_WS_MAC
    settings = new QSettings("KDE.org", "Marble Desktop Globe");
#else
    settings = new QSettings("KDE.org", "Marble Desktop Globe");
#endif
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

void QtMarbleConfigDialog::readSettings()
{
    // Sync settings to make sure that we read the current settings.
    syncSettings();
    
    // View
    ui_viewSettings->kcfg_distanceUnit->setCurrentIndex( distanceUnit() );
    ui_viewSettings->kcfg_angleUnit->setCurrentIndex( angleUnit() );
    ui_viewSettings->kcfg_stillQuality->setCurrentIndex( stillQuality() );
    ui_viewSettings->kcfg_animationQuality->setCurrentIndex( animationQuality() );
    ui_viewSettings->kcfg_labelLocalization->setCurrentIndex( labelLocalization() );
    ui_viewSettings->kcfg_mapFont->setCurrentFont( mapFont() );
    
    // Navigation
    ui_navigationSettings->kcfg_dragLocation->setCurrentIndex( dragLocation() );
    ui_navigationSettings->kcfg_onStartup->setCurrentIndex( onStartup() );
    if( animateTargetVoyage() )
	ui_navigationSettings->kcfg_animateTargetVoyage->setCheckState( Qt::Checked );
    else
	ui_navigationSettings->kcfg_animateTargetVoyage->setCheckState( Qt::Unchecked );
    
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
    
    emit settingsChanged();
}

void QtMarbleConfigDialog::writeSettings()
{
    syncSettings();
    
    settings->beginGroup( "View" );
	settings->setValue( "distanceUnit", ui_viewSettings->kcfg_distanceUnit->currentIndex() );
	settings->setValue( "angleUnit", ui_viewSettings->kcfg_angleUnit->currentIndex() );
	settings->setValue( "stillQuality", ui_viewSettings->kcfg_stillQuality->currentIndex() );
	settings->setValue( "animationQuality", ui_viewSettings->kcfg_animationQuality->currentIndex() );
	settings->setValue( "labelLocalization", ui_viewSettings->kcfg_labelLocalization->currentIndex() );
	settings->setValue( "mapFont", ui_viewSettings->kcfg_mapFont->currentFont() );
    settings->endGroup();
    
    settings->beginGroup( "Navigation" );
	settings->setValue( "dragLocation", ui_navigationSettings->kcfg_dragLocation->currentIndex() );
	settings->setValue( "onStartup", ui_navigationSettings->kcfg_onStartup->currentIndex() );
	if( ui_navigationSettings->kcfg_animateTargetVoyage->checkState() == Qt::Checked )
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
	
    emit settingsChanged();
}

Marble::DistanceUnit QtMarbleConfigDialog::distanceUnit()
{
    return (Marble::DistanceUnit) settings->value( "View/distanceUnit", Marble::Metric ).toInt();
}

Marble::AngleUnit QtMarbleConfigDialog::angleUnit()
{
    return (Marble::AngleUnit) settings->value( "View/angleUnit", Marble::DMSDegree ).toInt();
}

Marble::MapQuality QtMarbleConfigDialog::stillQuality()
{
    return (Marble::MapQuality) settings->value( "View/stillQuality", Marble::High ).toInt();
}

Marble::MapQuality QtMarbleConfigDialog::animationQuality()
{
    return (Marble::MapQuality) settings->value( "View/animationQuality", Marble::Low ).toInt();
}

int QtMarbleConfigDialog::labelLocalization()
{
    return settings->value( "View/labelLocalization" , Marble::Native ).toInt();
}

QFont QtMarbleConfigDialog::mapFont()
{
    return settings->value( "View/mapFont", QApplication::font() ).value<QFont>();
}

int QtMarbleConfigDialog::dragLocation()
{
    return settings->value( "Navigation/dragLocation", Marble::KeepAxisVertically ).toInt();
}

int QtMarbleConfigDialog::onStartup()
{
    return settings->value( "Navigation/onStartup", Marble::ShowHomeLocation ).toInt();
}

bool QtMarbleConfigDialog::animateTargetVoyage()
{
    return settings->value( "Navigation/animateTargetVoyage", false ).toBool();
}

int QtMarbleConfigDialog::volatileTileCacheLimit()
{
    return settings->value( "Cache/volatileTileCacheLimit", 30 ).toInt();
}

int QtMarbleConfigDialog::persistentTileCacheLimit()
{
    return settings->value( "Cache/persistentTileCacheLimit", 300 ).toInt();
}

QString QtMarbleConfigDialog::proxyUrl()
{
    return settings->value( "Cache/proxyUrl", "http://" ).toString();
}

int QtMarbleConfigDialog::proxyPort()
{
    return settings->value( "Cache/proxyPort", 8080 ).toInt();
}

QString QtMarbleConfigDialog::proxyUser()
{
    return settings->value( "Cache/proxyUser", "" ).toString();
}

QString QtMarbleConfigDialog::proxyPass()
{
    return settings->value( "Cache/proxyPass", "" ).toString();
}

bool QtMarbleConfigDialog::proxyHttp()
{
    return settings->value( "Cache/proxyHttp", "" ).toBool();
}

bool QtMarbleConfigDialog::proxySocks5()
{
    return settings->value( "Cache/proxySocks5", "" ).toBool();
}

bool QtMarbleConfigDialog::proxyAuth()
{
    return settings->value( "Cache/proxyAuth", false ).toBool();
}

#include "QtMarbleConfigDialog.moc"
