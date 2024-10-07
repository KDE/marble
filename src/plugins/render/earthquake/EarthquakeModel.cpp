// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Utku Aydin <utkuaydin34@gmail.com>
//

#include "EarthquakeModel.h"
#include "EarthquakeItem.h"

#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleGlobal.h"
#include "MarbleModel.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QUrl>

namespace Marble
{

EarthquakeModel::EarthquakeModel(const MarbleModel *marbleModel, QObject *parent)
    : AbstractDataPluginModel("earthquake", marbleModel, parent)
    , m_minMagnitude(0.0)
    , m_startDate(QDateTime::fromString("2006-02-04", "yyyy-MM-dd"))
    , m_endDate(QDateTime::currentDateTime())
{
    // nothing to do
}

EarthquakeModel::~EarthquakeModel() = default;

void EarthquakeModel::setMinMagnitude(double minMagnitude)
{
    m_minMagnitude = minMagnitude;
}

void EarthquakeModel::setStartDate(const QDateTime &startDate)
{
    m_startDate = startDate;
}

void EarthquakeModel::setEndDate(const QDateTime &endDate)
{
    m_endDate = endDate;
}

void EarthquakeModel::getAdditionalItems(const GeoDataLatLonAltBox &box, qint32 number)
{
    if (marbleModel()->planetId() != QLatin1StringView("earth")) {
        return;
    }

    const QString geonamesUrl(
        QLatin1StringView("http://api.geonames.org/earthquakesJSON") + QLatin1StringView("?north=") + QString::number(box.north() * RAD2DEG)
        + QLatin1StringView("&south=") + QString::number(box.south() * RAD2DEG) + QLatin1StringView("&east=") + QString::number(box.east() * RAD2DEG)
        + QLatin1StringView("&west=") + QString::number(box.west() * RAD2DEG) + QLatin1StringView("&date=") + m_endDate.toString("yyyy-MM-dd")
        + QLatin1StringView("&maxRows=") + QString::number(number) + QLatin1StringView("&username=marble") + QLatin1StringView("&formatted=true"));
    downloadDescriptionFile(QUrl(geonamesUrl));
}

void EarthquakeModel::parseFile(const QByteArray &file)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file);
    QJsonValue earthquakesValue = jsonDoc.object().value(QStringLiteral("earthquakes"));

    // Parse if any result exists
    if (earthquakesValue.isArray()) {
        // Add items to the list
        QList<AbstractDataPluginItem *> items;

        QJsonArray earthquakeArray = earthquakesValue.toArray();
        for (int earthquakeIndex = 0; earthquakeIndex < earthquakeArray.size(); ++earthquakeIndex) {
            QJsonObject levelObject = earthquakeArray[earthquakeIndex].toObject();

            // Converting earthquake's properties from JSON to appropriate types
            const QString eqid = levelObject.value(QStringLiteral("eqid")).toString(); // Earthquake's ID
            const double longitude = levelObject.value(QStringLiteral("lng")).toDouble();
            const double latitude = levelObject.value(QStringLiteral("lat")).toDouble();
            const double magnitude = levelObject.value(QStringLiteral("magnitude")).toDouble();
            const QString dateString = levelObject.value(QStringLiteral("datetime")).toString();
            const QDateTime date = QDateTime::fromString(dateString, QStringLiteral("yyyy-MM-dd hh:mm:ss"));
            const double depth = levelObject.value(QStringLiteral("depth")).toDouble();

            if (date <= m_endDate && date >= m_startDate && magnitude >= m_minMagnitude) {
                if (!itemExists(eqid)) {
                    // If it does not exists, create it
                    GeoDataCoordinates coordinates(longitude, latitude, 0.0, GeoDataCoordinates::Degree);
                    auto item = new EarthquakeItem(this);
                    item->setId(eqid);
                    item->setCoordinate(coordinates);
                    item->setMagnitude(magnitude);
                    item->setDateTime(date);
                    item->setDepth(depth);
                    items << item;
                }
            }
        }

        addItemsToList(items);
    }
}

}

#include "moc_EarthquakeModel.cpp"
