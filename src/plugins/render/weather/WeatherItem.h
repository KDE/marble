//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef WEATHERITEM_H
#define WEATHERITEM_H

// Marble
#include "AbstractDataPluginItem.h"

// Qt
#include <QtCore/QMap>

class QString;
class QAction;
class QDate;

namespace Marble
{
    
class WeatherData;

class WeatherItemPrivate;

/**
 * This is the class painting a weather item on the screen. So it is a subclass of
 * AbstractDataItem.
 */
class WeatherItem : public AbstractDataPluginItem
{
    Q_OBJECT
 public:
    WeatherItem( QObject *parent = 0 );
    ~WeatherItem();
    
    QAction *action();
    
    QString itemType() const;

    /**
     * Test if the item wants to request @p type again.
     */
    virtual bool request( const QString& type );
    
    /**
     * Returns the provider of the weather information.
     */
    virtual QString service() const = 0;
     
    bool initialized();
    
    virtual void addDownloadedFile( const QString& url, const QString& type ) = 0;
                         
    bool operator<( const AbstractDataPluginItem *other ) const;
    
    QString stationName() const;
    void setStationName( const QString& name );
    
    WeatherData currentWeather() const;
    void setCurrentWeather( const WeatherData& weather );

    QMap<QDate, WeatherData> forecastWeather() const;
    void setForecastWeather( const QMap<QDate, WeatherData>& forecasts );

    /**
     * Adds additional forecasts to the list. If there are multiple forecasts for one day,
     * it will choose the most recent (as of pubDate).
     */
    void addForecastWeather( const QList<WeatherData>& forecasts );
    
    quint8 priority() const;
    void setPriority( quint8 priority );

    virtual QString creditHtml() const;

    void setSettings( const QHash<QString, QVariant>& settings );
    
 public Q_SLOTS:
    void openBrowser();

 private:
    Q_DISABLE_COPY(WeatherItem)
    WeatherItemPrivate * const d;
    friend class WeatherItemPrivate;
};

} // namespace Marble

#endif // WEATHERITEM_H
