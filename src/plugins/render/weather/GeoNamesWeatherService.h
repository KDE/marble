//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Valery Kharitonov <kharvd@gmail.com>
//

#ifndef GEONAMESWEATHERSERVICE_H
#define GEONAMESWEATHERSERVICE_H

#include "AbstractWeatherService.h"

namespace Marble
{

class GeoNamesWeatherService : public AbstractWeatherService
{
    Q_OBJECT

 public:
    GeoNamesWeatherService( QObject *parent );
    ~GeoNamesWeatherService();

 public Q_SLOTS:
    void getAdditionalItems( const GeoDataLatLonAltBox& box,
                             const MarbleModel *model,
                             qint32 number = 10 );
    void parseFile( const QByteArray& file );

 private:
    void setupHashes();

    static QHash<QString, WeatherData::WeatherCondition> dayConditions;
    static QVector<WeatherData::WindDirection> windDirections;
};

} // namespace Marble

#endif // GEONAMESWEATHERSERVICE_H
