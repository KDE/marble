// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "BBCWeatherService.h"

// Marble
#include "BBCItemGetter.h"
#include "BBCStation.h"
#include "BBCWeatherItem.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleDirs.h"
#include "StationListParser.h"
#include "WeatherModel.h"

// Qt
#include <QUrl>

using namespace Marble;

BBCWeatherService::BBCWeatherService(const MarbleModel *model, QObject *parent)
    : AbstractWeatherService(model, parent)
    , m_parsingStarted(false)
    , m_parser(nullptr)
    , m_itemGetter(new BBCItemGetter(this))
{
    qRegisterMetaType<BBCStation>("BBCStation");
}

BBCWeatherService::~BBCWeatherService() = default;

void BBCWeatherService::setFavoriteItems(const QStringList &favorite)
{
    if (favoriteItems() != favorite) {
        m_parsingStarted = false;

        delete m_itemGetter;
        m_itemGetter = new BBCItemGetter(this);

        AbstractWeatherService::setFavoriteItems(favorite);
    }
}

void BBCWeatherService::getAdditionalItems(const GeoDataLatLonAltBox &box, qint32 number)
{
    if (!m_parsingStarted) {
        m_parsingStarted = true;

        m_parser = new StationListParser(this);
        m_parser->setPath(MarbleDirs::path(QStringLiteral("weather/bbc-stations.xml")));
        connect(m_parser, SIGNAL(finished()), this, SLOT(fetchStationList()));
        if (m_parser->wait(100)) {
            m_parser->start(QThread::IdlePriority);
        }
    }

    m_itemGetter->setSchedule(box, number);
}

void BBCWeatherService::getItem(const QString &id)
{
    if (id.startsWith(QLatin1StringView("bbc"))) {
        BBCStation const station = m_itemGetter->station(id);
        if (station.bbcId() > 0) {
            createItem(station);
        }
    }
}

void BBCWeatherService::fetchStationList()
{
    if (!m_parser) {
        return;
    }

    connect(m_itemGetter, &BBCItemGetter::foundStation, this, &BBCWeatherService::createItem);

    m_stationList = m_parser->stationList();
    m_itemGetter->setStationList(m_stationList);

    delete m_parser;
    m_parser = nullptr;
}

void BBCWeatherService::createItem(const BBCStation &station)
{
    auto item = new BBCWeatherItem(this);
    item->setMarbleWidget(marbleWidget());
    item->setBbcId(station.bbcId());
    item->setCoordinate(station.coordinate());
    item->setPriority(station.priority());
    item->setStationName(station.name());

    Q_EMIT requestedDownload(item->observationUrl(), QStringLiteral("bbcobservation"), item);
    Q_EMIT requestedDownload(item->forecastUrl(), QStringLiteral("bbcforecast"), item);
}

#include "moc_BBCWeatherService.cpp"
