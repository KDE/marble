//
// This file is part of the Marble Desktop Globe.
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

// Qt
#include <QtCore/QDebug>
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QAction>
#include <QtGui/QBrush>
#include <QtGui/QFontMetrics>
#include <QtSvg/QSvgRenderer>

namespace Marble {
    
const QSize borderSpacing( 4, 4 );
const QSize imageSize( 28, 28 );
const double imageSizeRatio = ( (double) imageSize.width()) / ( (double) imageSize.height() );
const qint32 horizontalSpacing = 4;
const qint32 verticalSpacing = 2;

class WeatherItemPrivate {
 public:
    WeatherItemPrivate( WeatherItem *parent )
        : m_priority( 0 ),
          m_action( new QAction( "Weather", parent ) ),
          m_parent( parent ),
          m_temperatureSize()
    {
        if ( s_windDirections.isEmpty() ) {
            s_windDirections.insert( WeatherData::N, "N" );
            s_windDirections.insert( WeatherData::NNE, "NNE" );
            s_windDirections.insert( WeatherData::NE, "NE" );
            s_windDirections.insert( WeatherData::ENE, "ENE" );
            s_windDirections.insert( WeatherData::E, "E" );
            s_windDirections.insert( WeatherData::SSE, "SSE" );
            s_windDirections.insert( WeatherData::SE, "SE" );
            s_windDirections.insert( WeatherData::S, "ESE" );
            s_windDirections.insert( WeatherData::NNW, "NNW" );
            s_windDirections.insert( WeatherData::NW, "NW" );
            s_windDirections.insert( WeatherData::WNW, "WNW" );
            s_windDirections.insert( WeatherData::W, "W" );
            s_windDirections.insert( WeatherData::SSW, "SSW" );
            s_windDirections.insert( WeatherData::SW, "SW" );
            s_windDirections.insert( WeatherData::WSW, "WSW" );
            s_windDirections.insert( WeatherData::DirectionNotAvailable, "N/A" );
        }
    }
    
    void updateSize() {
        // Getting the string sizes.
        QFontMetrics metrics( s_font );
        m_temperatureSize
                = metrics.boundingRect( temperatureString() ).size() + QSize( 6, 0 );
        m_windSpeedSize
                = metrics.boundingRect( windSpeedString() ).size() + QSize( 6, 0 );

        // The list containing the size of all elements in the first row.
        QList<QSize> elementSize1;
        // The list conatining the size of all elements in the second row.
        QList<QSize> elementSize2;
        
        // Image size
        if ( isConditionShown() )
        {
            elementSize1.append( imageSize ) ;
        }
        
        // TextSize
        if ( isTemperatureShown() ) {
            elementSize1.append( m_temperatureSize );
        }
        
        // Wind
        if ( isWindSpeedShown() ) {
            // Append a second row.
            elementSize2.append( m_windSpeedSize );
            if ( isWindDirectionShown() ) {
                elementSize2.append( imageSize );
            }
        }
        else if ( isWindDirectionShown() ) {
            // Don't append second row
            elementSize1.append( imageSize );
        }
        
        // Getting the size of the first row.
        // The row should at least be able to contain a image.
        QSize row1Size( 0, imageSize.height() );
        foreach ( QSize size, elementSize1 ) {
            row1Size += QSize( size.width(), 0 );
            // The height of the first row is the maximum of the elements height.
            if ( size.height() > row1Size.height() ) {
                row1Size.setHeight( size.height() );
            }
        }

        // Getting the size of the second row.
        // The row should at least be able to contain a image.
        QSize row2Size( 0, imageSize.height() );
        foreach ( QSize size, elementSize2 ) {
            row2Size += QSize( size.width(), 0 );
            // The height of the second row is the maximum of the elements height.
            if ( size.height() > row2Size.height() ) {
                row2Size.setHeight( size.height() );
            }
        }
        
        // Calculating the overall size.
        QSize size;
        if ( !row1Size.isEmpty() ) {
            size += QSize( 0, row1Size.height() );
            if ( row1Size.width() > size.width() ) {
                size.setWidth( row1Size.width() );
            }
        }

        if ( !row2Size.isEmpty() ) {
            size += QSize( 0, row2Size.height() );
            if ( row2Size.width() > size.width() ) {
                size.setWidth( row2Size.width() );
            }
        }

        // The space between the rows
        if ( !row1Size.isEmpty() && !row2Size.isEmpty() ) {
            size += QSize( verticalSpacing, 0 );
        }

        // The border of the item
        size += QSize( borderSpacing.width(), borderSpacing.height() ) * 2;

        m_topRowRect = QRect( QPoint( borderSpacing.width(), borderSpacing.height() ),
                              row1Size );
        m_bottomRowRect = QRect( m_topRowRect.bottomLeft() + QPoint( 0, verticalSpacing ),
                                 row2Size );
        // The rects are allowed to take the full width.
        m_topRowRect.setWidth( size.width() );
        m_bottomRowRect.setWidth( size.width() );
        // Leave out the border
        m_topRowRect.setRight( m_topRowRect.right() - borderSpacing.width() * 2 );
        m_bottomRowRect.setRight( m_bottomRowRect.right() - borderSpacing.width() * 2 );
        
        m_parent->setSize( size );
    }
    
    bool isConditionShown() {
        return m_currentWeather.hasValidCondition()
               && m_settings.value( "showCondition", showConditionDefault ).toBool();
    }
    
    bool isTemperatureShown() {
        return m_currentWeather.hasValidTemperature()
               && m_settings.value( "showTemperature", showTemperatureDefault ).toBool();
    }
    
    bool isWindDirectionShown() {
        return m_currentWeather.hasValidWindDirection()
               && m_settings.value( "showWindDirection", showWindDirectionDefault ).toBool();
    }
    
    bool isWindSpeedShown() {
        return m_currentWeather.hasValidWindSpeed()
               && m_settings.value( "showWindSpeed", showWindSpeedDefault ).toBool();
    }

    QString temperatureString() {
        WeatherData::TemperatureUnit temperatureUnit
                = (WeatherData::TemperatureUnit) m_settings.value( "temperatureUnit",
                                                                   WeatherData::Celsius ).toInt();
        return m_currentWeather.temperatureString( temperatureUnit );
    }

    QString windSpeedString() {
        WeatherData::SpeedUnit speedUnit
                = (WeatherData::SpeedUnit) m_settings.value( "windSpeedUnit",
                                                             WeatherData::kph ).toInt();
        return m_currentWeather.windSpeedString( speedUnit );
    }
    
    WeatherData m_currentWeather;
    int m_priority;
    QAction *m_action;
    WeatherItem *m_parent;
    // The size of the string displaying the temperature.
    QSize m_temperatureSize;
    // The size of the string displaying the wind speed.
    QSize m_windSpeedSize;
    QString m_stationName;
    QHash<QString,QVariant> m_settings;
    QRect m_topRowRect;
    QRect m_bottomRowRect;
    
    static QFont s_font;
    static QSvgRenderer s_windIcons;
    static QHash<WeatherData::WindDirection, QString> s_windDirections;
};

// FIXME: Fonts to be defined globally
#ifdef Q_OS_MACX
    QFont WeatherItemPrivate::s_font = QFont( "Sans Serif", 10 );
#else
    QFont WeatherItemPrivate::s_font = QFont( "Sans Serif", 8 );
#endif

QSvgRenderer WeatherItemPrivate::s_windIcons( MarbleDirs::path( "weather/wind-arrows.svgz" ) );

QHash<WeatherData::WindDirection, QString> WeatherItemPrivate::s_windDirections
        = QHash<WeatherData::WindDirection, QString>();

WeatherItem::WeatherItem( QObject *parent )
    : AbstractDataPluginItem( parent ),
    d( new WeatherItemPrivate( this ) )
{
    setCacheMode( MarbleGraphicsItem::ItemCoordinateCache );
    d->updateSize();
}

WeatherItem::~WeatherItem() {
    delete d;
}

QAction *WeatherItem::action() {
    return d->m_action;
}

QString WeatherItem::itemType() const {
    return QString( "weatherItem" );
}
 
bool WeatherItem::initialized() {
    WeatherData current = currentWeather();
    return d->isConditionShown()
           || d->isTemperatureShown()
           || d->isWindDirectionShown()
           || d->isWindSpeedShown();
}

void WeatherItem::paint( GeoPainter *painter, ViewportParams *viewport,
                         const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    // The rect for the first row.
    QRect topRow = d->m_topRowRect;
    // The rect for the second row.
    QRect bottomRow = d->m_bottomRowRect;
    
    painter->save();
    // FIXME: I think background rendering should be done in MarbleGraphicsItem.
    // This would be able to merge background with abstractfloatitem (on branch).
    painter->setBrush( QBrush( QColor( 192, 192, 192, 192 ) ) );
    painter->setPen( Qt::SolidLine );
    
    painter->setFont( d->s_font );
    painter->drawRoundedRect( QRectF( QPointF( 0, 0 ), size() ), 4, 4 );
    
    // Condition
    if ( d->isConditionShown() ) {
        painter->drawPixmap( topRow.topLeft(),
                             d->m_currentWeather.icon().pixmap( imageSize ) );
        topRow.setLeft( topRow.left() + imageSize.width() );
    }

    // Temperature
    if ( d->isTemperatureShown() ) {
        QRect temperatureRect;
        Qt::Alignment alignment;
        temperatureRect.setWidth( d->m_temperatureSize.width() );
        temperatureRect.setHeight( topRow.height() );
        // If we have a two line layout or condition and temperature is alone in line 1,
        // align temperature right.
        if ( d->isWindSpeedShown() || !d->isWindDirectionShown() ) {
            temperatureRect.moveTopRight( topRow.topRight() );
            alignment = Qt::AlignVCenter | Qt::AlignRight;
            topRow.setRight( topRow.right() - d->m_temperatureSize.width() );
        }
        else {
            temperatureRect.moveTopLeft( topRow.topLeft() );
            alignment = Qt::AlignCenter;
            topRow.setLeft( topRow.left() + d->m_temperatureSize.width() );
        }
        painter->drawText( temperatureRect,
                           alignment,
                           d->temperatureString() );
    }

    // Calculate the real size of the wind direction icon.
    QString windDirectionString = d->s_windDirections.value( d->m_currentWeather.windDirection() );
    QRect windDirectionRect;
    if( d->isWindDirectionShown() ) {
        QSizeF windDirectionSizeF = d->s_windIcons.boundsOnElement( windDirectionString ).size();
        double windDirectionRation = windDirectionSizeF.width() / windDirectionSizeF.height();
        if ( windDirectionRation >= imageSizeRatio ) {
            windDirectionRect.setWidth( imageSize.width() );
            windDirectionRect.setHeight( imageSize.width() / windDirectionRation );
            windDirectionRect.moveTop( ( imageSize.height() - windDirectionRect.height() ) / 2 );
        }
        else {
            windDirectionRect.setHeight( imageSize.height() );
            windDirectionRect.setWidth( imageSize.height() * windDirectionRation );
            windDirectionRect.moveLeft( ( imageSize.width() - windDirectionRect.width() ) / 2 );
        }
    }

    if ( d->isWindSpeedShown() ) {
        // Append second row

        // Paint the wind direction icon.
        if ( d->isWindDirectionShown() ) {
            windDirectionRect.moveTopLeft( windDirectionRect.topLeft() + bottomRow.topLeft() );
            d->s_windIcons.render( painter,
                                   d->s_windDirections.value( d->m_currentWeather.windDirection() ),
                                   windDirectionRect );
            bottomRow.setLeft( bottomRow.left() + imageSize.width() );
        }

        // Paint the wind speed string.
        QRect windSpeedRect;
        windSpeedRect.setWidth( d->m_windSpeedSize.width() );
        windSpeedRect.setHeight( bottomRow.height() );
        windSpeedRect.moveTopRight( bottomRow.topRight() );
        painter->drawText( windSpeedRect,
                           Qt::AlignVCenter | Qt::AlignRight,
                           d->windSpeedString() );
        bottomRow.setRight( bottomRow.right() - d->m_windSpeedSize.width() );
    }
    else if ( d->isWindDirectionShown() ) {
        // Don't append second row
        // Paint the wind direction icon.
        windDirectionRect.moveTopLeft( windDirectionRect.topLeft() + topRow.topLeft() );
        d->s_windIcons.render( painter,
                               d->s_windDirections.value( d->m_currentWeather.windDirection() ),
                               windDirectionRect );
        topRow.setLeft( topRow.left() + imageSize.width() );
    }

    painter->restore();
}

bool WeatherItem::operator<( const AbstractDataPluginItem *other ) const {
    const WeatherItem *weatherItem = qobject_cast<const WeatherItem *>(other);
    if( weatherItem ) {
        return ( priority() > ( (WeatherItem *) other )->priority() );
    }
    else {
        return false;
    }
}

QString WeatherItem::stationName() const {
    return d->m_stationName;
}

void WeatherItem::setStationName( const QString& name ) {
    d->m_action->setText( name );
    d->m_stationName = name;
    update();
}

WeatherData WeatherItem::currentWeather() const {
    return d->m_currentWeather;
}

void WeatherItem::setCurrentWeather( const WeatherData &weather ) {
    d->m_currentWeather = weather;
    d->updateSize();
    update();
}

quint8 WeatherItem::priority() const {
    return d->m_priority;
}

void WeatherItem::setPriority( quint8 priority ) {
    d->m_priority = priority;
}

void WeatherItem::setSettings( QHash<QString, QVariant> settings ) {
    if ( d->m_settings == settings ) {
        return;
    }
    d->m_settings = settings;

    d->updateSize();
    update();
}

} // namespace Marble

#include "WeatherItem.moc"
