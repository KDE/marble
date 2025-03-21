// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "WeatherModel.h"

// Qt
#include <QTimer>
#include <QUrl>

// Marble
#include "AbstractDataPluginItem.h"
#include "BBCWeatherService.h"
#include "FakeWeatherService.h"
#include "GeoNamesWeatherService.h"
#include "MarbleModel.h"
#include "WeatherItem.h"

using namespace Marble;

WeatherModel::WeatherModel(const MarbleModel *marbleModel, QObject *parent)
    : AbstractDataPluginModel(QStringLiteral("weather"), marbleModel, parent)
{
    registerItemProperties(WeatherItem::staticMetaObject);

    // addService( new FakeWeatherService( marbleModel(), this ) );
    addService(new BBCWeatherService(marbleModel, this));
    addService(new GeoNamesWeatherService(marbleModel, this));

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &AbstractDataPluginModel::clear);

    // Default interval = 3 hours
    setUpdateInterval(3);

    m_timer->start();
}

WeatherModel::~WeatherModel() = default;

void WeatherModel::setFavoriteItems(const QStringList &list)
{
    if (favoriteItems() != list) {
        for (AbstractWeatherService *service : m_services) {
            service->setFavoriteItems(list);
        }

        AbstractDataPluginModel::setFavoriteItems(list);
    }
}

void WeatherModel::setUpdateInterval(quint32 hours)
{
    quint32 msecs = hours * 60 * 60 * 1000;
    m_timer->setInterval(msecs);
}

void WeatherModel::downloadItemData(const QUrl &url, const QString &type, AbstractDataPluginItem *item)
{
    AbstractDataPluginItem *existingItem = findItem(item->id());
    if (!existingItem) {
        auto weatherItem = qobject_cast<WeatherItem *>(item);
        if (weatherItem) {
            weatherItem->request(type);
        }

        downloadItem(url, type, item);
        addItemToList(item);
    } else {
        if (existingItem != item)
            item->deleteLater();

        auto existingWeatherItem = qobject_cast<WeatherItem *>(existingItem);
        if (existingWeatherItem && existingWeatherItem->request(type)) {
            downloadItem(url, type, existingItem);
            addItemToList(existingItem);
        }
    }
}

void WeatherModel::getAdditionalItems(const GeoDataLatLonAltBox &box, qint32 number)
{
    for (AbstractWeatherService *service : m_services) {
        service->getAdditionalItems(box, number);
    }
}

void WeatherModel::getItem(const QString &id)
{
    for (AbstractWeatherService *service : std::as_const(m_services)) {
        service->getItem(id);
    }
}

void WeatherModel::parseFile(const QByteArray &file)
{
    for (AbstractWeatherService *service : std::as_const(m_services)) {
        service->parseFile(file);
    }
}

void WeatherModel::downloadDescriptionFileRequested(const QUrl &url)
{
    downloadDescriptionFile(url);
}

void WeatherModel::setMarbleWidget(MarbleWidget *widget)
{
    for (AbstractWeatherService *service : std::as_const(m_services)) {
        service->setMarbleWidget(widget);
    }
}

void WeatherModel::addService(AbstractWeatherService *service)
{
    service->setFavoriteItems(favoriteItems());

    connect(service, &AbstractWeatherService::createdItems, this, &AbstractDataPluginModel::addItemsToList);
    connect(service, &AbstractWeatherService::requestedDownload, this, &WeatherModel::downloadItemData);
    connect(service, &AbstractWeatherService::downloadDescriptionFileRequested, this, &WeatherModel::downloadDescriptionFileRequested);

    m_services.append(service);
}

#include "moc_WeatherModel.cpp"
