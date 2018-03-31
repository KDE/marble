//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
// Copyright 2012      Illya Kovalevskyy  <illya.kovalevskyy@gmail.com>
//

// Self
#include "WeatherItem.h"

// Marble
#include "GeoPainter.h"
#include "MarbleDirs.h"
#include "WeatherData.h"
#include "weatherGlobal.h"
#include "FrameGraphicsItem.h"
#include "LabelGraphicsItem.h"
#include "MarbleGraphicsGridLayout.h"
#include "WidgetGraphicsItem.h"
#include "TinyWebBrowser.h"
#include "MarbleDebug.h"
#include "MarbleWidget.h"
#include "layers/PopupLayer.h"

// Qt
#include <QDate>
#include <QHash>
#include <QString>
#include <QAction>
#include <QIcon>
#include <QFont>
#include <QPushButton>
#include <QSvgRenderer>

namespace Marble
{
    
const QSize borderSpacing( 4, 4 );
const QSize imageSize( 28, 28 );
const double imageSizeRatio = ( (double) imageSize.width()) / ( (double) imageSize.height() );

class WeatherItemPrivate
{
    Q_DECLARE_TR_FUNCTIONS(WeatherItemPrivate)
    
 public:
    WeatherItemPrivate( WeatherItem *parent )
        : m_marbleWidget( nullptr ),
          m_priority( 0 ),
          m_browserAction( tr( "Weather" ), parent ),
          m_favoriteAction( parent ),
          m_parent( parent ),
          m_frameItem( m_parent ),
          m_conditionLabel( &m_frameItem ),
          m_temperatureLabel( &m_frameItem ),
          m_windDirectionLabel( &m_frameItem ),
          m_windSpeedLabel( &m_frameItem ),
          m_favoriteButton( &m_frameItem )
    {
        // Setting minimum sizes
        m_temperatureLabel.setMinimumSize( QSizeF( 0, imageSize.height() ) );
        m_windSpeedLabel.setMinimumSize( QSizeF( 0, imageSize.height() ) );

        QPushButton *button = new QPushButton();
        button->setStyleSheet( "border-style: outset;" );
        button->setIcon(QIcon(QStringLiteral(":/icons/bookmarks.png")));
        button->setFixedSize( 22, 22 );
        button->setFlat( true );
        button->setCheckable( true );

        m_favoriteButton.setWidget( button );

        // Layouting the item
        MarbleGraphicsGridLayout *topLayout = new MarbleGraphicsGridLayout( 1, 1 );
        parent->setLayout( topLayout );
        topLayout->addItem( &m_frameItem, 0, 0 );

        MarbleGraphicsGridLayout *gridLayout = new MarbleGraphicsGridLayout( 2, 3 );
        gridLayout->setAlignment( Qt::AlignCenter );
        gridLayout->setSpacing( 4 );
        m_frameItem.setLayout( gridLayout );
        m_frameItem.setFrame( FrameGraphicsItem::RoundedRectFrame );

        gridLayout->addItem( &m_conditionLabel, 0, 0 );
        gridLayout->addItem( &m_temperatureLabel, 0, 1 );
        gridLayout->setAlignment( &m_temperatureLabel, Qt::AlignRight | Qt::AlignVCenter );
        gridLayout->addItem( &m_windDirectionLabel, 1, 0 );
        gridLayout->addItem( &m_windSpeedLabel, 1, 1 );
        gridLayout->setAlignment( &m_windSpeedLabel, Qt::AlignRight | Qt::AlignVCenter );
        gridLayout->addItem( &m_favoriteButton, 0, 2 );

        updateLabels();
    }

    ~WeatherItemPrivate()
    {
    }

    void updateToolTip()
    {
        QLocale locale = QLocale::system();
        QString toolTip;
        toolTip += tr( "Station: %1\n" ).arg( m_parent->stationName() );
        if ( m_currentWeather.hasValidCondition() && m_currentWeather.hasValidTemperature() )
            toolTip += QString( "%2, %3\n" ).arg( m_currentWeather.conditionString() )
                                           .arg( temperatureString() );
        else if ( m_currentWeather.hasValidCondition() )
            toolTip += QString( "%2\n" ).arg( m_currentWeather.conditionString() );
        else if ( m_currentWeather.hasValidTemperature() )
            toolTip += QString( "%2\n" ).arg( temperatureString() );

        if ( m_currentWeather.hasValidWindSpeed() && m_currentWeather.hasValidWindDirection() )
            toolTip += tr( "Wind: %4, %5\n", "Wind: WindSpeed, WindDirection" )
                    .arg( windSpeedString( ) )
                    .arg( m_currentWeather.windDirectionString() );
        else if ( m_currentWeather.hasValidWindSpeed() )
            toolTip += tr( "Wind: %4\n", "Wind: WindSpeed" )
                    .arg( m_currentWeather.windSpeedString() );
        else if ( m_currentWeather.hasValidWindDirection() )
            toolTip += tr( "Wind: %4\n", "Wind: WindDirection" )
                    .arg( m_currentWeather.windDirectionString() );

        if ( m_currentWeather.hasValidPressure() && m_currentWeather.hasValidPressureDevelopment() )
            toolTip += tr( "Pressure: %6, %7", "Pressure: Pressure, Development" )
                    .arg( pressureString() )
                    .arg( m_currentWeather.pressureDevelopmentString() );
        else if ( m_currentWeather.hasValidPressure() )
            toolTip += tr( "Pressure: %6", "Pressure: Pressure" )
                    .arg( pressureString() );
        else if ( m_currentWeather.hasValidPressureDevelopment() )
            toolTip += tr( "Pressure %7", "Pressure Development" )
                    .arg( m_currentWeather.pressureDevelopmentString() );

        if ( !m_forecastWeather.isEmpty() ) {
            toolTip += QLatin1Char('\n');

            QDate const minDate = QDate::currentDate();
            for( const WeatherData& data: m_forecastWeather ) {
                QDate date = data.dataDate();
                if( date >= minDate
                    && data.hasValidCondition()
                    && data.hasValidMinTemperature()
                    && data.hasValidMaxTemperature() )
                {
                    toolTip += QLatin1Char('\n') +
                               tr( "%1: %2, %3 to %4", "DayOfWeek: Condition, MinTemp to MaxTemp" )
                               .arg( locale.standaloneDayName( date.dayOfWeek() ) )
                               .arg( data.conditionString() )
                               .arg( data.minTemperatureString( temperatureUnit() ) )
                               .arg( data.maxTemperatureString( temperatureUnit() ) );
                }
            }
        }

        m_parent->setToolTip( toolTip );
    }

    void updateLabels()
    {
        if ( isConditionShown() ) {
            m_conditionLabel.setImage( m_currentWeather.icon(), imageSize );
        }
        else {
            m_conditionLabel.clear();
        }

        if ( isTemperatureShown() ) {
            m_temperatureLabel.setText( temperatureString() );
        }
        else {
            m_temperatureLabel.clear();
        }

        if ( isWindDirectionShown() ) {
            QString windDirectionString = m_currentWeather.windDirectionString();
            QSizeF windDirectionImageSize;
            QSvgRenderer s_windIcons(MarbleDirs::path(QStringLiteral("weather/wind-arrows.svgz")));
            QSizeF windDirectionSizeF = s_windIcons.boundsOnElement( windDirectionString ).size();
            double windDirectionRatio = windDirectionSizeF.width() / windDirectionSizeF.height();
            if ( windDirectionRatio >= imageSizeRatio ) {
                windDirectionImageSize.setWidth( imageSize.width() );
                windDirectionImageSize.setHeight( imageSize.width() / windDirectionRatio );
            }
            else {
                windDirectionImageSize.setHeight( imageSize.height() );
                windDirectionImageSize.setWidth( imageSize.height() * windDirectionRatio );
            }

            QImage windArrow( windDirectionImageSize.toSize(), QImage::Format_ARGB32 );
            windArrow.fill( Qt::transparent );
            QPainter painter( &windArrow );
            s_windIcons.render( &painter, windDirectionString );
            m_windDirectionLabel.setImage( windArrow );
        }
        else {
            m_windDirectionLabel.clear();
        }

        if ( isWindSpeedShown() ) {
            m_windSpeedLabel.setText( windSpeedString() );
        }
        else {
            m_windSpeedLabel.clear();
        }

        m_parent->update();
    }

    void updateFavorite()
    {
        QStringList items = m_settings.value(QStringLiteral("favoriteItems")).toString()
                                        .split(QLatin1Char(','), QString::SkipEmptyParts);
        bool favorite = items.contains( m_parent->id() );

        m_favoriteButton.setVisible( favorite );
        m_favoriteAction.setText( favorite ? tr( "Remove from Favorites" )
                                           : tr( "Add to Favorites" ) );

        if ( m_parent->isFavorite() != favorite ) {
            m_parent->setFavorite( favorite );
        }

        m_parent->update();
    }
    
    bool isConditionShown() const
    {
        return m_currentWeather.hasValidCondition()
               && m_settings.value(QStringLiteral("showCondition"), showConditionDefault).toBool();
    }
    
    bool isTemperatureShown() const
    {
        return m_currentWeather.hasValidTemperature()
               && m_settings.value(QStringLiteral("showTemperature"), showTemperatureDefault).toBool();
    }
    
    bool isWindDirectionShown() const
    {
        return m_currentWeather.hasValidWindDirection()
               && m_settings.value(QStringLiteral("showWindDirection"), showWindDirectionDefault).toBool();
    }
    
    bool isWindSpeedShown() const
    {
        return m_currentWeather.hasValidWindSpeed()
               && m_settings.value(QStringLiteral("showWindSpeed"), showWindSpeedDefault).toBool();
    }

    QString temperatureString() const
    {
        WeatherData::TemperatureUnit tUnit = temperatureUnit();
        return m_currentWeather.temperatureString( tUnit );
    }

    WeatherData::TemperatureUnit temperatureUnit() const
    {
        WeatherData::TemperatureUnit tUnit
                = (WeatherData::TemperatureUnit) m_settings.value(QStringLiteral("temperatureUnit"),
                                                                   WeatherData::Celsius ).toInt();
        return tUnit;
    }

    QString windSpeedString() const
    {
        return m_currentWeather.windSpeedString( speedUnit() );
    }

    WeatherData::SpeedUnit speedUnit() const
    {
        return (WeatherData::SpeedUnit) m_settings.value(QStringLiteral("windSpeedUnit"),
                                                          WeatherData::kph ).toInt();
    }

    QString pressureString() const
    {
        return m_currentWeather.pressureString( pressureUnit() );
    }

    WeatherData::PressureUnit pressureUnit() const
    {
        return (WeatherData::PressureUnit) m_settings.value(QStringLiteral("pressureUnit"),
                                                             WeatherData::HectoPascal ).toInt();
    }

    MarbleWidget *m_marbleWidget;

    WeatherData m_currentWeather;
    QMap<QDate, WeatherData> m_forecastWeather;

    quint8 m_priority;
    QAction m_browserAction;
    QAction m_favoriteAction;
    WeatherItem *m_parent;
    QString m_stationName;
    QHash<QString,QVariant> m_settings;
    
    static QFont s_font;

    // Labels and Layout
    // We are not the owner of these items.
    FrameGraphicsItem  m_frameItem;
    LabelGraphicsItem  m_conditionLabel;
    LabelGraphicsItem  m_temperatureLabel;
    LabelGraphicsItem  m_windDirectionLabel;
    LabelGraphicsItem  m_windSpeedLabel;
    WidgetGraphicsItem m_favoriteButton;
};

// FIXME: Fonts to be defined globally
#ifdef Q_OS_MACX
    QFont WeatherItemPrivate::s_font = QFont( QStringLiteral( "Sans Serif" ), 10 );
#else
    QFont WeatherItemPrivate::s_font = QFont( QStringLiteral( "Sans Serif" ), 8 );
#endif

WeatherItem::WeatherItem(QObject *parent )
    : AbstractDataPluginItem( parent ),
    d( new WeatherItemPrivate( this ) )
{
    setCacheMode( ItemCoordinateCache );
}

WeatherItem::WeatherItem(MarbleWidget* widget, QObject *parent )
    : AbstractDataPluginItem( parent ),
    d( new WeatherItemPrivate( this ) )
{
    setCacheMode( ItemCoordinateCache );
    d->m_marbleWidget = widget;
}

WeatherItem::~WeatherItem()
{
    delete d;
}

QAction *WeatherItem::action()
{
    disconnect( &d->m_browserAction, SIGNAL(triggered()),
                this,        SLOT(openBrowser()) );
    connect(    &d->m_browserAction, SIGNAL(triggered()),
                this,        SLOT(openBrowser()) );
    return &d->m_browserAction;
}

bool WeatherItem::request( const QString& type )
{
    Q_UNUSED( type )
    return true;
}
 
bool WeatherItem::initialized() const
{
    return d->isConditionShown()
           || d->isTemperatureShown()
           || d->isWindDirectionShown()
           || d->isWindSpeedShown();
}

bool WeatherItem::operator<( const AbstractDataPluginItem *other ) const
{
    const WeatherItem *weatherItem = qobject_cast<const WeatherItem *>(other);
    if( weatherItem ) {
        return ( priority() > weatherItem->priority() );
    }
    else {
        return false;
    }
}

QString WeatherItem::stationName() const
{
    return d->m_stationName;
}

void WeatherItem::setStationName( const QString& name )
{
    if ( name != d->m_stationName ) {
        d->m_browserAction.setText( name );
        d->m_stationName = name;
        d->updateToolTip();
        d->updateLabels();
        emit stationNameChanged();
    }
}

WeatherData WeatherItem::currentWeather() const
{
    return d->m_currentWeather;
}

void WeatherItem::setCurrentWeather( const WeatherData &weather )
{
    d->m_currentWeather = weather;
    d->updateToolTip();
    d->updateLabels();
    emit updated();
    emit descriptionChanged();
    emit imageChanged();
    emit temperatureChanged();
}

QMap<QDate, WeatherData> WeatherItem::forecastWeather() const
{
    return d->m_forecastWeather;
}

void WeatherItem::setForecastWeather( const QMap<QDate, WeatherData>& forecasts )
{
    d->m_forecastWeather = forecasts;

    d->updateToolTip();
    emit updated();
}

void WeatherItem::addForecastWeather( const QList<WeatherData>& forecasts )
{
    for( const WeatherData& data: forecasts ) {
        QDate date = data.dataDate();
        WeatherData other = d->m_forecastWeather.value( date );
        if ( !other.isValid() ) {
            d->m_forecastWeather.insert( date, data );
        }
        else if ( other.publishingTime() < data.publishingTime() ) {
            d->m_forecastWeather.remove( date );
            d->m_forecastWeather.insert( date, data );
        }
    }

    // Remove old items
    QDate const minDate = QDate::currentDate();

    QMap<QDate, WeatherData>::iterator it = d->m_forecastWeather.begin();

    while( it != d->m_forecastWeather.end() ) {
        if ( it.key() < minDate ) {
            it = d->m_forecastWeather.erase( it );
        } else {
            ++it;
        }
    }

    d->updateToolTip();
    emit updated();
}

quint8 WeatherItem::priority() const
{
    return d->m_priority;
}

void WeatherItem::setPriority( quint8 priority )
{
    d->m_priority = priority;
}

void WeatherItem::setSettings( const QHash<QString, QVariant>& settings )
{
    if ( d->m_settings == settings ) {
        return;
    }

    d->m_settings = settings;

    d->updateToolTip();
    d->updateLabels();
    d->updateFavorite();
}

void WeatherItem::setMarbleWidget(MarbleWidget *widget)
{
    d->m_marbleWidget = widget;
}

void WeatherItem::openBrowser()
{
    if (d->m_marbleWidget) {
        PopupLayer *popup = d->m_marbleWidget->popupLayer();
        popup->setCoordinates( coordinate(), Qt::AlignRight | Qt::AlignVCenter );
        popup->setSize(QSizeF(630, 580)); // +10 pixels for the width
        popup->popup();

        QFile weatherHtmlFile(QStringLiteral(":/marble/weather/weather.html"));
        if ( !weatherHtmlFile.open(QIODevice::ReadOnly) ) {
            return;
        }

        QString templateHtml = weatherHtmlFile.readAll();
        popup->setContent( createFromTemplate(templateHtml) );
    }
}

QString WeatherItem::createFromTemplate(const QString &templateHtml)
{
    QString html = templateHtml;
    QLocale locale = QLocale::system();
    html.replace("%city_name%", stationName());

    if (!d->m_currentWeather.iconSource().isEmpty()) {
        html.replace("%weather_situation%",
                     QLatin1String("<img src=\"file://") + d->m_currentWeather.iconSource() + QLatin1String("\" />"));
    } else {
        html.remove("%weather_situation%");
    }

    html.replace("%current_temp%", d->temperatureString());
    html.replace("%current_condition%", d->m_currentWeather.conditionString());
    html.replace("%wind_direction%", d->m_currentWeather.windDirectionString());
    html.replace("%wind_speed%", d->m_currentWeather.windSpeedString());
    html.replace("%humidity_level%", d->m_currentWeather.humidityString());
    html.replace("%publish_time%", d->m_currentWeather.publishingTime().toString());

    if(d->m_forecastWeather.size() < 1) {
        html.replace("%forecast_available%", "none");
    } else {
        html.replace("%forecast_available%", "block");
    }

    int forecastNumber = 0;

    for ( const WeatherData &forecast: d->m_forecastWeather ) {
        forecastNumber++;
        const QString suffix = QString::number(forecastNumber);
        QDate date = forecast.dataDate();
        html.replace(QLatin1String("%day_f") + suffix + QLatin1Char('%'),
                     locale.standaloneDayName(date.dayOfWeek()));
        html.replace(QLatin1String("%weather_situation_f") + suffix + QLatin1Char('%'),
                     QLatin1String("file://")+forecast.iconSource());
        html.replace(QLatin1String("%max_temp_f") + suffix + QLatin1Char('%'),
                      forecast.maxTemperatureString(WeatherData::Celsius));
        html.replace(QLatin1String("%min_temp_f") + suffix + QLatin1Char('%'),
                      forecast.minTemperatureString(WeatherData::Celsius));
        html.replace(QLatin1String("%condition_f") + suffix + QLatin1Char('%'), forecast.conditionString());
        html.replace(QLatin1String("%wind_direction_f") + suffix + QLatin1Char('%'), forecast.windDirectionString());
        html.replace(QLatin1String("%wind_speed_f") + suffix + QLatin1Char('%'), forecast.windSpeedString());
        html.replace(QLatin1String("%publish_time_f") + suffix + QLatin1Char('%'), forecast.publishingTime().toString());
    }

    return html;
}

QList<QAction*> WeatherItem::actions()
{
    QList<QAction*> result;
    result << &d->m_browserAction;

    disconnect( &d->m_favoriteAction, SIGNAL(triggered()),
                this,        SLOT(toggleFavorite()) );
    connect(    &d->m_favoriteAction, SIGNAL(triggered()),
                this,        SLOT(toggleFavorite()) );

    result << &d->m_favoriteAction;

    return result;
}

QString WeatherItem::description() const
{
    return d->m_currentWeather.toHtml( WeatherData::Celsius, WeatherData::kph, WeatherData::Bar );
}

QString WeatherItem::image() const
{
    return d->m_currentWeather.iconSource();
}

double WeatherItem::temperature() const
{
    return d->m_currentWeather.hasValidTemperature()
         ? d->m_currentWeather.temperature( WeatherData::Celsius )
         : 0.0;
}

} // namespace Marble

#include "moc_WeatherItem.cpp"
