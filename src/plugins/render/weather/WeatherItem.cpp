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
#include "WeatherItem.h"

// Marble
#include "GeoPainter.h"
#include "MarbleDirs.h"
#include "WeatherData.h"
#include "weatherGlobal.h"
#include "FrameGraphicsItem.h"
#include "LabelGraphicsItem.h"
#include "MarbleGraphicsGridLayout.h"
#include "TinyWebBrowser.h"
#include "MarbleDebug.h"

// Qt
#include <QtCore/QCoreApplication>
#include <QtCore/QDate>
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QAction>
#include <QtGui/QBrush>
#include <QtGui/QFontMetrics>
#include <QtSvg/QSvgRenderer>

namespace Marble
{
    
const QSize borderSpacing( 4, 4 );
const QSize imageSize( 28, 28 );
const double imageSizeRatio = ( (double) imageSize.width()) / ( (double) imageSize.height() );
const qint32 horizontalSpacing = 4;
const qint32 verticalSpacing = 2;

class WeatherItemPrivate
{
    Q_DECLARE_TR_FUNCTIONS(WeatherItemPrivate)
    
 public:
    WeatherItemPrivate( WeatherItem *parent )
        : m_priority( 0 ),
          m_action( new QAction( tr( "Weather" ), parent ) ),
          m_browser( 0 ),
          m_parent( parent ),
          m_frameItem( new FrameGraphicsItem( m_parent ) ),
          m_conditionLabel( new LabelGraphicsItem( m_frameItem ) ),
          m_temperatureLabel( new LabelGraphicsItem( m_frameItem ) ),
          m_windDirectionLabel( new LabelGraphicsItem( m_frameItem ) ),
          m_windSpeedLabel( new LabelGraphicsItem( m_frameItem ) )
    {
        // Setting minimum sizes
        m_temperatureLabel->setMinimumSize( QSizeF( 0, imageSize.height() ) );
        m_windSpeedLabel->setMinimumSize( QSizeF( 0, imageSize.height() ) );

        // Layouting the item
        MarbleGraphicsGridLayout *topLayout = new MarbleGraphicsGridLayout( 1, 1 );
        parent->setLayout( topLayout );
        topLayout->addItem( m_frameItem, 0, 0 );

        MarbleGraphicsGridLayout *gridLayout = new MarbleGraphicsGridLayout( 2, 2 );
        gridLayout->setAlignment( Qt::AlignCenter );
        gridLayout->setSpacing( 4 );
        m_frameItem->setLayout( gridLayout );
        m_frameItem->setFrame( FrameGraphicsItem::RoundedRectFrame );

        gridLayout->addItem( m_conditionLabel, 0, 0 );
        gridLayout->addItem( m_temperatureLabel, 0, 1 );
        gridLayout->setAlignment( m_temperatureLabel, Qt::AlignRight | Qt::AlignVCenter );
        gridLayout->addItem( m_windDirectionLabel, 1, 0 );
        gridLayout->addItem( m_windSpeedLabel, 1, 1 );
        gridLayout->setAlignment( m_windSpeedLabel, Qt::AlignRight | Qt::AlignVCenter );

        updateLabels();
    }

    ~WeatherItemPrivate() {
        delete m_browser;
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
            toolTip += '\n';

            QDate minDate = QDate::currentDate();
            minDate.addDays( -1 );
            foreach( const WeatherData& data, m_forecastWeather ) {
                QDate date = data.dataDate();
                if( date >= minDate
                    && data.hasValidCondition()
                    && data.hasValidMinTemperature()
                    && data.hasValidMaxTemperature() )
                {
                    toolTip += '\n';
                    toolTip += tr( "%1: %2, %3 to %4", "DayOfWeek: Condition, MinTemp to MaxTemp" )
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
            m_conditionLabel->setImage( m_currentWeather.icon(), imageSize );
        }
        else {
            m_conditionLabel->clear();
        }

        if ( isTemperatureShown() ) {
            m_temperatureLabel->setText( temperatureString() );
        }
        else {
            m_temperatureLabel->clear();
        }

        if ( isWindDirectionShown() ) {
            QString windDirectionString = m_currentWeather.windDirectionString();
            QSizeF windDirectionImageSize;
            QSvgRenderer s_windIcons( MarbleDirs::path( "weather/wind-arrows.svgz" ) );
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
            m_windDirectionLabel->setImage( windArrow );
        }
        else {
            m_windDirectionLabel->clear();
        }

        if ( isWindSpeedShown() ) {
            m_windSpeedLabel->setText( windSpeedString() );
        }
        else {
            m_windSpeedLabel->clear();
        }

        m_parent->update();
    }
    
    bool isConditionShown()
    {
        return m_currentWeather.hasValidCondition()
               && m_settings.value( "showCondition", showConditionDefault ).toBool();
    }
    
    bool isTemperatureShown()
    {
        return m_currentWeather.hasValidTemperature()
               && m_settings.value( "showTemperature", showTemperatureDefault ).toBool();
    }
    
    bool isWindDirectionShown()
    {
        return m_currentWeather.hasValidWindDirection()
               && m_settings.value( "showWindDirection", showWindDirectionDefault ).toBool();
    }
    
    bool isWindSpeedShown()
    {
        return m_currentWeather.hasValidWindSpeed()
               && m_settings.value( "showWindSpeed", showWindSpeedDefault ).toBool();
    }

    QString temperatureString()
    {
        WeatherData::TemperatureUnit tUnit = temperatureUnit();
        return m_currentWeather.temperatureString( tUnit );
    }

    WeatherData::TemperatureUnit temperatureUnit()
    {
        WeatherData::TemperatureUnit tUnit
                = (WeatherData::TemperatureUnit) m_settings.value( "temperatureUnit",
                                                                   WeatherData::Celsius ).toInt();
        return tUnit;
    }

    QString windSpeedString()
    {
        return m_currentWeather.windSpeedString( speedUnit() );
    }

    WeatherData::SpeedUnit speedUnit()
    {
        return (WeatherData::SpeedUnit) m_settings.value( "windSpeedUnit",
                                                          WeatherData::kph ).toInt();
    }

    QString pressureString()
    {
        return m_currentWeather.pressureString( pressureUnit() );
    }

    WeatherData::PressureUnit pressureUnit()
    {
        return (WeatherData::PressureUnit) m_settings.value( "pressureUnit",
                                                             WeatherData::HectoPascal ).toInt();
    }
    
    WeatherData m_currentWeather;
    QMap<QDate, WeatherData> m_forecastWeather;

    int m_priority;
    QAction *m_action;
    TinyWebBrowser *m_browser;
    WeatherItem *m_parent;
    QString m_stationName;
    QHash<QString,QVariant> m_settings;
    
    static QFont s_font;

    // Labels and Layout
    // We are not the owner of these items.
    FrameGraphicsItem *m_frameItem;
    LabelGraphicsItem *m_conditionLabel;
    LabelGraphicsItem *m_temperatureLabel;
    LabelGraphicsItem *m_windDirectionLabel;
    LabelGraphicsItem *m_windSpeedLabel;
};

// FIXME: Fonts to be defined globally
#ifdef Q_OS_MACX
    QFont WeatherItemPrivate::s_font = QFont( "Sans Serif", 10 );
#else
    QFont WeatherItemPrivate::s_font = QFont( "Sans Serif", 8 );
#endif

WeatherItem::WeatherItem( QObject *parent )
    : AbstractDataPluginItem( parent ),
    d( new WeatherItemPrivate( this ) )
{
    setCacheMode( MarbleGraphicsItem::ItemCoordinateCache );
}

WeatherItem::~WeatherItem()
{
    delete d;
}

QAction *WeatherItem::action()
{
    disconnect( d->m_action, SIGNAL( triggered() ),
                this,        SLOT( openBrowser() ) );
    connect(    d->m_action, SIGNAL( triggered() ),
                this,        SLOT( openBrowser() ) );
    return d->m_action;
}

QString WeatherItem::itemType() const
{
    return QString( "weatherItem" );
}

bool WeatherItem::request( const QString& type )
{
    Q_UNUSED( type )
    return false;
}
 
bool WeatherItem::initialized()
{
    WeatherData current = currentWeather();
    return d->isConditionShown()
           || d->isTemperatureShown()
           || d->isWindDirectionShown()
           || d->isWindSpeedShown();
}

bool WeatherItem::operator<( const AbstractDataPluginItem *other ) const
{
    const WeatherItem *weatherItem = qobject_cast<const WeatherItem *>(other);
    if( weatherItem ) {
        return ( priority() > ( (WeatherItem *) other )->priority() );
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
    d->m_action->setText( name );
    d->m_stationName = name;
    d->updateToolTip();
    d->updateLabels();
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
    foreach( const WeatherData& data, forecasts ) {
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
    QDate minDate = QDate::currentDate();
    minDate.addDays( -1 );

    QMap<QDate, WeatherData>::iterator it = d->m_forecastWeather.begin();

    while( it != d->m_forecastWeather.end() ) {
        if ( it.key() < minDate ) {
            d->m_forecastWeather.remove( it.key() );
        }
        it++;
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

QString WeatherItem::creditHtml() const
{
    return QString();
}

void WeatherItem::setSettings( const QHash<QString, QVariant>& settings )
{
    if ( d->m_settings == settings ) {
        return;
    }
    d->m_settings = settings;

    d->updateToolTip();
    d->updateLabels();
}

void WeatherItem::openBrowser()
{
    QLocale locale = QLocale::system();
    if( !d->m_browser ) {
        d->m_browser = new TinyWebBrowser();
    }
    QString html;
    html += "<html>";
    html += "<body>";
    html += "<h1>" + tr( "Weather for %1" ).arg( stationName() ) + "</h1>";
    if ( d->m_currentWeather.isValid() ) {
        html += "<h2>" + tr( "Current Observation" ) + "</h2>";
        html += d->m_currentWeather.toHtml( d->temperatureUnit(),
                                            d->speedUnit(),
                                            d->pressureUnit() );
    }
    if ( !d->m_forecastWeather.isEmpty() ) {
        html += "<h2>" + tr( "Forecasts" ) + "</h2>";
    }
    foreach ( const WeatherData& data, d->m_forecastWeather ) {
        QDate date = data.dataDate();
        html += "<h3>" + locale.standaloneDayName( date.dayOfWeek() ) + "</h3>";
        html += data.toHtml( d->temperatureUnit(),
                             d->speedUnit(),
                             d->pressureUnit() );
    }
    QString credit = creditHtml();
    if ( !credit.isEmpty() ) {
        html += "<p>";
        html += creditHtml();
        html += "</p>";
    }
    html += "</body>";
    html += "</html>";

    d->m_browser->setHtml( html );
    d->m_browser->show();
}

} // namespace Marble

#include "WeatherItem.moc"
