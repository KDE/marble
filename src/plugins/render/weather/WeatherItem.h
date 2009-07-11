//
// This file is part of the Marble Desktop Globe.
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

class QString;
class QAction;

namespace Marble {
    
class GeoPainter;
class ViewportParams;
class GeoSceneLayer;
class WeatherData;

class WeatherItemPrivate;

/**
 * This is the class painting a weather item on the screen. So it is a subclass of
 * AbstractDataItem.
 */
class WeatherItem : public AbstractDataPluginItem {
    Q_OBJECT
 public:
    WeatherItem( QObject *parent = 0 );
    ~WeatherItem();
    
    QAction *action();
    
    QString itemType() const;
    
    /**
     * Returns the provider of the weather information.
     */
    virtual QString service() const = 0;
     
    bool initialized();
    
    virtual void addDownloadedFile( const QString& url, const QString& type ) = 0;
    
    void paint( GeoPainter *painter, ViewportParams *viewport,
                const QString& renderPos, GeoSceneLayer * layer = 0 );
                         
    bool operator<( const AbstractDataPluginItem *other ) const;
    
    QString stationName() const;
    void setStationName( const QString& name );
    
    WeatherData currentWeather() const;
    void setCurrentWeather( const WeatherData& weather );
    
    quint8 priority() const;
    void setPriority( quint8 priority );

    void setSettings( QHash<QString, QVariant> settings );
    
    // Forecasts to appear later
 private:
    Q_DISABLE_COPY(WeatherItem);
    WeatherItemPrivate * const d;
    friend class WeatherItemPrivate;
};

} // namespace Marble

#endif // WEATHERITEM_H
