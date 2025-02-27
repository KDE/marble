// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef BBCWEATHERSERVICE_H
#define BBCWEATHERSERVICE_H

#include "AbstractWeatherService.h"

#include <QList>
#include <QStringList>

namespace Marble
{

class BBCItemGetter;
class BBCStation;
class GeoDataLatLonAltBox;
class StationListParser;

class BBCWeatherService : public AbstractWeatherService
{
    Q_OBJECT

public:
    explicit BBCWeatherService(const MarbleModel *model, QObject *parent);
    ~BBCWeatherService() override;

    void setFavoriteItems(const QStringList &favorite) override;

public Q_SLOTS:
    void getAdditionalItems(const GeoDataLatLonAltBox &box, qint32 number = 10) override;
    void getItem(const QString &id) override;

private Q_SLOTS:
    void fetchStationList();
    void createItem(const BBCStation &station);

private:
    QList<BBCStation> m_stationList;
    bool m_parsingStarted;
    StationListParser *m_parser = nullptr;
    BBCItemGetter *m_itemGetter = nullptr;
};

} // namespace Marble

#endif // BBCWEATHERSERVICE_H
