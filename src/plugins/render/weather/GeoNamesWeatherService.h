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
#include "WeatherData.h"

class QJsonObject;

namespace Marble
{

class GeoNamesWeatherService : public AbstractWeatherService
{
    Q_OBJECT

 public:
    explicit GeoNamesWeatherService( const MarbleModel *model, QObject *parent );
    ~GeoNamesWeatherService() override;

 public Q_SLOTS:
    void getAdditionalItems( const GeoDataLatLonAltBox& box,
                             qint32 number = 10 ) override;
    void getItem( const QString &id ) override;
    void parseFile( const QByteArray& file ) override;

 private:
    AbstractDataPluginItem* parse(const QJsonObject &weatherObservationObject);
    static void setupHashes();

    static QHash<QString, WeatherData::WeatherCondition> dayConditions;
    static QVector<WeatherData::WindDirection> windDirections;
};

} // namespace Marble

#endif // GEONAMESWEATHERSERVICE_H
