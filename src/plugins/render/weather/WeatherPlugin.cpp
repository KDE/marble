//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "WeatherPlugin.h"

// Marble
#include "ui_WeatherConfigWidget.h"
#include "WeatherData.h"
#include "WeatherModel.h"
#include "PluginAboutDialog.h"
#include "MarbleDirs.h"
#include "MarbleLocale.h"
#include "global.h"
#include "weatherGlobal.h"
#include "MarbleDebug.h"

// Qt
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QIcon>
#include <QtGui/QPushButton>

using namespace Marble;
/* TRANSLATOR Marble::WeatherPlugin */

const quint32 numberOfStationsPerFetch = 20;

WeatherPlugin::WeatherPlugin()
    : m_isInitialized( false ),
      m_icon(),
      m_aboutDialog( 0 ),
      m_configDialog( 0 ),
      ui_configWidget( 0 ),
      m_settings()
{
    setNameId( "weather" );
        
    // Plugin is enabled by default
    setEnabled( true );
    // Plugin is not visible by default
    setVisible( false );
    
    connect( this, SIGNAL( settingsChanged( QString ) ),
             this, SLOT( updateItemSettings() ) );

    setSettings( QHash<QString,QVariant>() );
}

WeatherPlugin::~WeatherPlugin()
{
    delete m_aboutDialog;
    delete m_configDialog;
    delete ui_configWidget;
}

void WeatherPlugin::initialize()
{
    WeatherModel *model = new WeatherModel( pluginManager(), this );
    setModel( model );
    updateItemSettings();
    setNumberOfItems( numberOfStationsPerFetch );
    m_isInitialized = true;
}

bool WeatherPlugin::isInitialized() const
{
    return m_isInitialized;
}

QString WeatherPlugin::name() const
{
    return tr( "Weather" );
}

QString WeatherPlugin::guiString() const
{
    return tr( "&Weather" );
}

QString WeatherPlugin::description() const
{
    return tr( "Download weather information from many weather stations all around the world" );
}

QIcon WeatherPlugin::icon() const
{
    return m_icon;
}

QDialog *WeatherPlugin::aboutDialog()
{
    if ( !m_aboutDialog ) {
        m_aboutDialog = new PluginAboutDialog();
        m_aboutDialog->setName( "Weather Plugin" );
        m_aboutDialog->setVersion( "0.1" );
        // FIXME: Can we store this string for all of Marble
        m_aboutDialog->setAboutText( tr( "<br />(c) 2009 The Marble Project<br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>" ) );
        QList<Author> authors;
        Author bholst;
        bholst.name = "Bastian Holst";
        bholst.task = tr( "Developer" );
        bholst.email = "bastianholst@gmx.de";
        authors.append( bholst );
        m_aboutDialog->setAuthors( authors );
        m_aboutDialog->setDataText( tr( "Supported by backstage.bbc.co.uk.\nWeather data from UK MET Office" ) );
        m_icon.addFile( MarbleDirs::path( "weather/weather-clear.png" ) );
        m_aboutDialog->setPixmap( m_icon.pixmap( 62, 62 ) );
    }
    return m_aboutDialog;
}

QDialog *WeatherPlugin::configDialog()
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::WeatherConfigWidget;
        ui_configWidget->setupUi( m_configDialog );
        readSettings();
        connect( ui_configWidget->m_buttonBox, SIGNAL( accepted() ),
                                              SLOT( writeSettings() ) );
        connect( ui_configWidget->m_buttonBox, SIGNAL( rejected() ),
                                              SLOT( readSettings() ) );
        QPushButton *applyButton = ui_configWidget->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL( clicked() ),
                 this,        SLOT( writeSettings() ) );
    }
    return m_configDialog;
}

QHash<QString,QVariant> WeatherPlugin::settings() const
{
    return m_settings;
}

void WeatherPlugin::setSettings( QHash<QString,QVariant> settings )
{
    
    // Check if all fields are filled and fill them with default values.
    // Information
    if ( !settings.contains( "showCondition" ) ) {
        settings.insert( "showCondition", showConditionDefault );
    }
    
    if ( !settings.contains( "showTemperature" ) ) {
        settings.insert( "showTemperature", showTemperatureDefault );
    }
    
    if ( !settings.contains( "showWindDirection" ) ) {
        settings.insert( "showWindDirection", showWindDirectionDefault );
    }
    
    if ( !settings.contains( "showWindSpeed" ) ) {
        settings.insert( "showWindSpeed", showWindSpeedDefault );
    }
    
    // Units
    // The default units depend on the global measure system.
    MarbleLocale *locale = MarbleGlobal::getInstance()->locale();
    if ( !settings.contains( "temperatureUnit" ) ) {
        int temperatureUnit;
        if ( locale->measureSystem() == QLocale::MetricSystem ) {
            temperatureUnit = WeatherData::Celsius;
        }
        else {
            temperatureUnit = WeatherData::Fahrenheit;
        }
        settings.insert( "temperatureUnit", temperatureUnit );
    }
    
    if ( !settings.contains( "windSpeedUnit" ) ) {
        int windSpeedUnit;
        if ( locale->measureSystem() == QLocale::MetricSystem ) {
            windSpeedUnit = WeatherData::kph;
        }
        else {
            windSpeedUnit = WeatherData::mph;
        }
        settings.insert( "windSpeedUnit", windSpeedUnit );
    }
    
    if ( !settings.contains( "pressureUnit" ) ) {
        int pressureUnit;
        if ( locale->measureSystem() == QLocale::MetricSystem ) {
            pressureUnit = WeatherData::HectoPascal;
        }
        else {
            pressureUnit = WeatherData::inchHg;
        }
        settings.insert( "pressureUnit", pressureUnit );
    }
    
    m_settings = settings;
    readSettings();
    emit settingsChanged( nameId() );
}

void WeatherPlugin::readSettings()
{
    if ( !m_configDialog ) {
        return;
    }
    
    // Information
    if ( m_settings.value( "showCondition" ).toBool() )
        ui_configWidget->m_weatherConditionBox->setCheckState( Qt::Checked );
    else
        ui_configWidget->m_weatherConditionBox->setCheckState( Qt::Unchecked );

    if ( m_settings.value( "showTemperature" ).toBool() )
        ui_configWidget->m_temperatureBox->setCheckState( Qt::Checked );
    else
        ui_configWidget->m_temperatureBox->setCheckState( Qt::Unchecked );

    if ( m_settings.value( "showWindDirection" ).toBool() )
        ui_configWidget->m_windDirectionBox->setCheckState( Qt::Checked );
    else
        ui_configWidget->m_windDirectionBox->setCheckState( Qt::Unchecked );

    if ( m_settings.value( "showWindSpeed" ).toBool() )
        ui_configWidget->m_windSpeedBox->setCheckState( Qt::Checked );
    else
        ui_configWidget->m_windSpeedBox->setCheckState( Qt::Unchecked );

    // Units
    ui_configWidget->m_temperatureComboBox
        ->setCurrentIndex( m_settings.value( "temperatureUnit" ).toInt() );
    
    ui_configWidget->m_windSpeedComboBox
        ->setCurrentIndex( m_settings.value( "windSpeedUnit" ).toInt() );

    ui_configWidget->m_pressureComboBox
        ->setCurrentIndex( m_settings.value( "pressureUnit" ).toInt() );
}

void WeatherPlugin::writeSettings()
{
    // Information
    m_settings.insert( "showCondition",
                       ui_configWidget->m_weatherConditionBox->checkState() == Qt::Checked );
    m_settings.insert( "showTemperature",
                       ui_configWidget->m_temperatureBox->checkState() == Qt::Checked );
    m_settings.insert( "showWindDirection",
                       ui_configWidget->m_windDirectionBox->checkState() == Qt::Checked );
    m_settings.insert( "showWindSpeed",
                       ui_configWidget->m_windSpeedBox->checkState() == Qt::Checked );

    // Units
    m_settings.insert( "temperatureUnit", ui_configWidget->m_temperatureComboBox->currentIndex() );
    m_settings.insert( "windSpeedUnit", ui_configWidget->m_windSpeedComboBox->currentIndex() );
    m_settings.insert( "pressureUnit", ui_configWidget->m_pressureComboBox->currentIndex() );

    emit settingsChanged( nameId() );
}

void WeatherPlugin::updateItemSettings()
{
    AbstractDataPluginModel *abstractModel = model();
    if( abstractModel != 0 ) {
        abstractModel->setItemSettings( m_settings );
    }
}

Q_EXPORT_PLUGIN2(WeatherPlugin, Marble::WeatherPlugin)

#include "WeatherPlugin.moc"
