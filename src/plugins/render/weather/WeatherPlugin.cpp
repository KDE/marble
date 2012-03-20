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
      m_updateInterval( 0 ),
      m_icon(),
      m_configDialog( 0 ),
      ui_configWidget( 0 ),
      m_settings()
{
    m_icon.addFile( MarbleDirs::path( "weather/weather-clear.png" ) );

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
    delete m_configDialog;
    delete ui_configWidget;
}

void WeatherPlugin::initialize()
{
    readSettings();

    WeatherModel *model = new WeatherModel( pluginManager(), this );

    setModel( model );
    updateSettings();
    updateItemSettings();

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

QString WeatherPlugin::nameId() const
{
    return "weather";
}

QString WeatherPlugin::version() const
{
    return "1.1";
}

QString WeatherPlugin::description() const
{
    return tr( "Download weather information from many weather stations all around the world" );
}

QString WeatherPlugin::copyrightYears() const
{
    return "2009, 2011";
}

QList<PluginAuthor> WeatherPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Bastian Holst", "bastianholst@gmx.de" )
            << PluginAuthor( "Valery Kharitonov", "kharvd@gmail.com" );
}

QString WeatherPlugin::aboutDataText() const
{
    return tr( "Supported by backstage.bbc.co.uk.\nWeather data from UK MET Office" );
}

QIcon WeatherPlugin::icon() const
{
    return m_icon;
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

void WeatherPlugin::setSettings( const QHash<QString,QVariant> &settings )
{
    m_settings = settings;

    // Check if all fields are filled and fill them with default values.
    // Information
    if ( !m_settings.contains( "showCondition" ) ) {
        m_settings.insert( "showCondition", showConditionDefault );
    }
    
    if ( !m_settings.contains( "showTemperature" ) ) {
        m_settings.insert( "showTemperature", showTemperatureDefault );
    }
    
    if ( !m_settings.contains( "showWindDirection" ) ) {
        m_settings.insert( "showWindDirection", showWindDirectionDefault );
    }
    
    if ( !m_settings.contains( "showWindSpeed" ) ) {
        m_settings.insert( "showWindSpeed", showWindSpeedDefault );
    }
    
    // Units
    // The default units depend on the global measure system.
    MarbleLocale *locale = MarbleGlobal::getInstance()->locale();
    if ( !m_settings.contains( "temperatureUnit" ) ) {
        int temperatureUnit;
        if ( locale->measurementSystem() == QLocale::MetricSystem ) {
            temperatureUnit = WeatherData::Celsius;
        }
        else {
            temperatureUnit = WeatherData::Fahrenheit;
        }
        m_settings.insert( "temperatureUnit", temperatureUnit );
    }
    
    if ( !m_settings.contains( "windSpeedUnit" ) ) {
        int windSpeedUnit;
        if ( locale->measurementSystem() == QLocale::MetricSystem ) {
            windSpeedUnit = WeatherData::kph;
        }
        else {
            windSpeedUnit = WeatherData::mph;
        }
        m_settings.insert( "windSpeedUnit", windSpeedUnit );
    }
    
    if ( !m_settings.contains( "pressureUnit" ) ) {
        int pressureUnit;
        if ( locale->measurementSystem() == QLocale::MetricSystem ) {
            pressureUnit = WeatherData::HectoPascal;
        }
        else {
            pressureUnit = WeatherData::inchHg;
        }
        m_settings.insert( "pressureUnit", pressureUnit );
    }
    
    readSettings();

    emit settingsChanged( nameId() );
    updateSettings();
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

    if ( m_settings.value( "onlyFavorites" ).toBool() )
        ui_configWidget->m_onlyFavoritesBox->setCheckState( Qt::Checked );
    else
        ui_configWidget->m_onlyFavoritesBox->setCheckState( Qt::Unchecked );

    // Units
    ui_configWidget->m_temperatureComboBox
        ->setCurrentIndex( m_settings.value( "temperatureUnit" ).toInt() );
    
    ui_configWidget->m_windSpeedComboBox
        ->setCurrentIndex( m_settings.value( "windSpeedUnit" ).toInt() );

    ui_configWidget->m_pressureComboBox
        ->setCurrentIndex( m_settings.value( "pressureUnit" ).toInt() );

    // Misc
    ui_configWidget->m_updateIntervalBox
        ->setValue( m_settings.value( "updateInterval", 3 ).toInt() );
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

    // Misc
    bool onlyFavorites = ( ui_configWidget->m_onlyFavoritesBox->checkState() == Qt::Checked );
    m_settings.insert( "onlyFavorites", onlyFavorites );

    m_updateInterval = ui_configWidget->m_updateIntervalBox->value();
    m_settings.insert( "updateInterval", m_updateInterval );

    emit settingsChanged( nameId() );
    updateSettings();
}

void WeatherPlugin::updateSettings()
{
    if ( model() ) {
        bool favoritesOnly = m_settings.value( "onlyFavorites", false ).toBool();
        QList<QString> favoriteItems = m_settings.value( "favoriteItems" ).toString()
                .split(",", QString::SkipEmptyParts);

        model()->setFavoriteItems( favoriteItems );
        setNumberOfItems( favoritesOnly ? favoriteItems.size() : numberOfStationsPerFetch );
        model()->setFavoriteItemsOnly( favoritesOnly );
    }
}

void WeatherPlugin::updateItemSettings()
{
    AbstractDataPluginModel *abstractModel = model();
    if( abstractModel != 0 ) {
        abstractModel->setItemSettings( m_settings );
    }
}

void WeatherPlugin::favoriteItemsChanged( const QStringList& favoriteItems )
{
    m_settings["favoriteItems"] = favoriteItems.join( "," );
    emit settingsChanged( nameId() );
    updateSettings();
}

Q_EXPORT_PLUGIN2(WeatherPlugin, Marble::WeatherPlugin)

#include "WeatherPlugin.moc"
