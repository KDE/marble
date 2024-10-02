// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "WeatherPlugin.h"

// Marble
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleGlobal.h"
#include "MarbleLocale.h"
#include "MarbleWidget.h"
#include "WeatherData.h"
#include "WeatherModel.h"
#include "ui_WeatherConfigWidget.h"
#include "weatherGlobal.h"

// Qt
#include <QDialog>
#include <QPushButton>

using namespace Marble;
/* TRANSLATOR Marble::WeatherPlugin */

const quint32 numberOfStationsPerFetch = 20;

WeatherPlugin::WeatherPlugin()
    : AbstractDataPlugin(nullptr)
    , m_configDialog(nullptr)
    , ui_configWidget(nullptr)
{
}

WeatherPlugin::WeatherPlugin(const MarbleModel *marbleModel)
    : AbstractDataPlugin(marbleModel)
    , m_updateInterval(0)
    , m_icon(MarbleDirs::path(QStringLiteral("weather/weather-clear.png")))
    , m_configDialog(nullptr)
    , ui_configWidget(nullptr)
    , m_settings()
{
    // Plugin is enabled by default
    setEnabled(true);
    // Plugin is not visible by default
    setVisible(false);

    connect(this, SIGNAL(settingsChanged(QString)), this, SLOT(updateItemSettings()));

    setSettings(QHash<QString, QVariant>());
}

WeatherPlugin::~WeatherPlugin()
{
    delete m_configDialog;
    delete ui_configWidget;
}

void WeatherPlugin::initialize()
{
    readSettings();

    auto model = new WeatherModel(marbleModel(), this);

    setModel(model);
    updateSettings();
    updateItemSettings();
}

QString WeatherPlugin::name() const
{
    return tr("Weather");
}

QString WeatherPlugin::guiString() const
{
    return tr("&Weather");
}

QString WeatherPlugin::nameId() const
{
    return QStringLiteral("weather");
}

QString WeatherPlugin::version() const
{
    return QStringLiteral("1.1");
}

QString WeatherPlugin::description() const
{
    return tr("Download weather information from many weather stations all around the world");
}

QString WeatherPlugin::copyrightYears() const
{
    return QStringLiteral("2009, 2011");
}

QList<PluginAuthor> WeatherPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>() << PluginAuthor(QStringLiteral("Bastian Holst"), QStringLiteral("bastianholst@gmx.de"))
                                 << PluginAuthor(QStringLiteral("Valery Kharitonov"), QStringLiteral("kharvd@gmail.com"));
}

QString WeatherPlugin::aboutDataText() const
{
    return tr("Supported by backstage.bbc.co.uk.\nWeather data from UK MET Office");
}

QIcon WeatherPlugin::icon() const
{
    return m_icon;
}

QDialog *WeatherPlugin::configDialog()
{
    if (!m_configDialog) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::WeatherConfigWidget;
        ui_configWidget->setupUi(m_configDialog);
        readSettings();
        connect(ui_configWidget->m_buttonBox, SIGNAL(accepted()), SLOT(writeSettings()));
        connect(ui_configWidget->m_buttonBox, SIGNAL(rejected()), SLOT(readSettings()));
        QPushButton *applyButton = ui_configWidget->m_buttonBox->button(QDialogButtonBox::Apply);
        connect(applyButton, SIGNAL(clicked()), this, SLOT(writeSettings()));
    }
    return m_configDialog;
}

QHash<QString, QVariant> WeatherPlugin::settings() const
{
    QHash<QString, QVariant> result = AbstractDataPlugin::settings();

    typedef QHash<QString, QVariant>::ConstIterator Iterator;
    Iterator end = m_settings.constEnd();
    for (Iterator iter = m_settings.constBegin(); iter != end; ++iter) {
        result.insert(iter.key(), iter.value());
    }

    return result;
}

void WeatherPlugin::setSettings(const QHash<QString, QVariant> &settings)
{
    AbstractDataPlugin::setSettings(settings);

    m_settings.insert(QStringLiteral("showCondition"), settings.value(QStringLiteral("showCondition"), showConditionDefault));
    m_settings.insert(QStringLiteral("showTemperature"), settings.value(QStringLiteral("showTemperature"), showTemperatureDefault));
    m_settings.insert(QStringLiteral("showWindDirection"), settings.value(QStringLiteral("showWindDirection"), showWindDirectionDefault));
    m_settings.insert(QStringLiteral("showWindSpeed"), settings.value(QStringLiteral("showWindSpeed"), showWindSpeedDefault));

    // Units
    // The default units depend on the global measure system.
    const MarbleLocale *locale = MarbleGlobal::getInstance()->locale();

    int temperatureUnit;
    if (locale->measurementSystem() == MarbleLocale::MetricSystem) {
        temperatureUnit = WeatherData::Celsius;
    } else {
        temperatureUnit = WeatherData::Fahrenheit;
    }
    m_settings.insert(QStringLiteral("temperatureUnit"), settings.value(QStringLiteral("temperatureUnit"), temperatureUnit));

    int windSpeedUnit;
    if (locale->measurementSystem() == MarbleLocale::MetricSystem) {
        windSpeedUnit = WeatherData::kph;
    } else {
        windSpeedUnit = WeatherData::mph;
    }
    m_settings.insert(QStringLiteral("windSpeedUnit"), settings.value(QStringLiteral("windSpeedUnit"), windSpeedUnit));

    int pressureUnit;
    if (locale->measurementSystem() == MarbleLocale::MetricSystem) {
        pressureUnit = WeatherData::HectoPascal;
    } else {
        pressureUnit = WeatherData::inchHg;
    }
    m_settings.insert(QStringLiteral("pressureUnit"), settings.value(QStringLiteral("pressureUnit"), pressureUnit));

    readSettings();

    Q_EMIT settingsChanged(nameId());
    updateSettings();
}

bool WeatherPlugin::eventFilter(QObject *object, QEvent *event)
{
    if (isInitialized()) {
        auto weatherModel = qobject_cast<WeatherModel *>(model());
        Q_ASSERT(weatherModel);
        auto widget = qobject_cast<MarbleWidget *>(object);
        if (widget) {
            weatherModel->setMarbleWidget(widget);
        }
    }

    return AbstractDataPlugin::eventFilter(object, event);
}

void WeatherPlugin::readSettings()
{
    if (!m_configDialog) {
        return;
    }

    // Information
    if (m_settings.value(QStringLiteral("showCondition")).toBool())
        ui_configWidget->m_weatherConditionBox->setCheckState(Qt::Checked);
    else
        ui_configWidget->m_weatherConditionBox->setCheckState(Qt::Unchecked);

    if (m_settings.value(QStringLiteral("showTemperature")).toBool())
        ui_configWidget->m_temperatureBox->setCheckState(Qt::Checked);
    else
        ui_configWidget->m_temperatureBox->setCheckState(Qt::Unchecked);

    if (m_settings.value(QStringLiteral("showWindDirection")).toBool())
        ui_configWidget->m_windDirectionBox->setCheckState(Qt::Checked);
    else
        ui_configWidget->m_windDirectionBox->setCheckState(Qt::Unchecked);

    if (m_settings.value(QStringLiteral("showWindSpeed")).toBool())
        ui_configWidget->m_windSpeedBox->setCheckState(Qt::Checked);
    else
        ui_configWidget->m_windSpeedBox->setCheckState(Qt::Unchecked);

    if (m_settings.value(QStringLiteral("onlyFavorites")).toBool())
        ui_configWidget->m_onlyFavoritesBox->setCheckState(Qt::Checked);
    else
        ui_configWidget->m_onlyFavoritesBox->setCheckState(Qt::Unchecked);

    // Units
    ui_configWidget->m_temperatureComboBox->setCurrentIndex(m_settings.value(QStringLiteral("temperatureUnit")).toInt());

    ui_configWidget->m_windSpeedComboBox->setCurrentIndex(m_settings.value(QStringLiteral("windSpeedUnit")).toInt());

    ui_configWidget->m_pressureComboBox->setCurrentIndex(m_settings.value(QStringLiteral("pressureUnit")).toInt());

    // Misc
    ui_configWidget->m_updateIntervalBox->setValue(m_settings.value(QStringLiteral("updateInterval"), 3).toInt());
}

void WeatherPlugin::writeSettings()
{
    // Information
    m_settings.insert(QStringLiteral("showCondition"), ui_configWidget->m_weatherConditionBox->checkState() == Qt::Checked);
    m_settings.insert(QStringLiteral("showTemperature"), ui_configWidget->m_temperatureBox->checkState() == Qt::Checked);
    m_settings.insert(QStringLiteral("showWindDirection"), ui_configWidget->m_windDirectionBox->checkState() == Qt::Checked);
    m_settings.insert(QStringLiteral("showWindSpeed"), ui_configWidget->m_windSpeedBox->checkState() == Qt::Checked);

    // Units
    m_settings.insert(QStringLiteral("temperatureUnit"), ui_configWidget->m_temperatureComboBox->currentIndex());
    m_settings.insert(QStringLiteral("windSpeedUnit"), ui_configWidget->m_windSpeedComboBox->currentIndex());
    m_settings.insert(QStringLiteral("pressureUnit"), ui_configWidget->m_pressureComboBox->currentIndex());

    // Misc
    bool onlyFavorites = (ui_configWidget->m_onlyFavoritesBox->checkState() == Qt::Checked);
    m_settings.insert(QStringLiteral("onlyFavorites"), onlyFavorites);

    m_updateInterval = ui_configWidget->m_updateIntervalBox->value();
    m_settings.insert(QStringLiteral("updateInterval"), m_updateInterval);

    Q_EMIT settingsChanged(nameId());
    updateSettings();
}

void WeatherPlugin::updateSettings()
{
    if (model()) {
        bool favoritesOnly = m_settings.value(QStringLiteral("onlyFavorites"), false).toBool();
        QList<QString> favoriteItems = m_settings.value(QStringLiteral("favoriteItems")).toString().split(QLatin1Char(','), Qt::SkipEmptyParts);

        model()->setFavoriteItems(favoriteItems);
        setNumberOfItems(favoritesOnly ? favoriteItems.size() : numberOfStationsPerFetch);
        model()->setFavoriteItemsOnly(favoritesOnly);
    }
}

void WeatherPlugin::updateItemSettings()
{
    AbstractDataPluginModel *abstractModel = model();
    if (abstractModel != nullptr) {
        abstractModel->setItemSettings(m_settings);
    }
}

void WeatherPlugin::favoriteItemsChanged(const QStringList &favoriteItems)
{
    m_settings[QStringLiteral("favoriteItems")] = favoriteItems.join(QLatin1Char(','));
    Q_EMIT settingsChanged(nameId());
    updateSettings();
}

#include "moc_WeatherPlugin.cpp"
