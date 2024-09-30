// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Utku Aydın <utkuaydin34@gmail.com>
//

#include "FoursquareModel.h"
#include "FoursquareItem.h"
#include "FoursquarePlugin.h"

#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleDebug.h"
#include "MarbleGlobal.h"
#include "MarbleMath.h"
#include "MarbleModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

namespace Marble
{

FoursquareModel::FoursquareModel(const MarbleModel *marbleModel, QObject *parent)
    : AbstractDataPluginModel("foursquare", marbleModel, parent)
{
    // Enjoy laziness
}

FoursquareModel::~FoursquareModel()
{
}

void FoursquareModel::getAdditionalItems(const GeoDataLatLonAltBox &box, qint32 number)
{
    if (marbleModel()->planetId() != QLatin1StringView("earth")) {
        return;
    }

    QString clientId = "YPRWSYFW1RVL4PJQ2XS5G14RTOGTHOKZVHC1EP5KCCCYQPZF";
    QString clientSecret = "5L2JDCAYQCEJWY5FNDU4A1RWATE4E5FIIXXRM41YBTFSERUH";

    QString apiUrl("https://api.foursquare.com/v2/venues/search");
    qreal const distanceLon = marbleModel()->planetRadius() * distanceSphere(box.west(), box.north(), box.east(), box.north());
    qreal const distanceLat = marbleModel()->planetRadius() * distanceSphere(box.west(), box.north(), box.west(), box.south());
    qreal const area = distanceLon * distanceLat;
    if (area > 10 * 1000 * KM2METER * KM2METER) {
        // Large area (> 10.000 km^2) => too large for bbox queries
        apiUrl += QLatin1StringView("?ll=") + QString::number(box.center().latitude(Marble::GeoDataCoordinates::Degree)) + QLatin1Char(',')
            + QString::number(box.center().longitude(Marble::GeoDataCoordinates::Degree)) + QLatin1StringView("&intent=checkin");
    } else {
        apiUrl += QLatin1StringView("?ne=") + QString::number(box.north(Marble::GeoDataCoordinates::Degree)) + QLatin1Char(',')
            + QString::number(box.east(Marble::GeoDataCoordinates::Degree)) + QLatin1StringView("&sw=")
            + QString::number(box.south(Marble::GeoDataCoordinates::Degree)) + QLatin1Char(',') + QString::number(box.west(Marble::GeoDataCoordinates::Degree))
            + QLatin1StringView("&intent=browse");
    }
    apiUrl += QLatin1StringView("&limit=") + QString::number(number) + QLatin1StringView("&client_id=") + clientId + QLatin1StringView("&client_secret=")
        + clientSecret + QLatin1StringView("&v=20120601");
    downloadDescriptionFile(QUrl(apiUrl));
}

void FoursquareModel::parseFile(const QByteArray &file)
{
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(file);
    const QJsonObject responseObject = jsonDoc.object().value(QStringLiteral("response")).toObject();
    const QJsonValue venuesValue = responseObject.value(QStringLiteral("response"));

    // Parse if any result exists
    if (venuesValue.isArray()) {
        // Add items to the list
        QList<AbstractDataPluginItem *> items;

        const QJsonArray venueArray = venuesValue.toArray();
        for (int venueIndex = 0; venueIndex < venueArray.size(); ++venueIndex) {
            const QJsonObject venueObject = venueArray[venueIndex].toObject();

            const QJsonObject firstCategoryObject = venueObject.value(QStringLiteral("categories")).toArray().at(0).toObject();

            const QString id = venueObject.value(QStringLiteral("id")).toString();
            const QString name = venueObject.value(QStringLiteral("name")).toString();
            const QString category = firstCategoryObject.value(QStringLiteral("name")).toString();
            const QJsonObject locationObject = venueObject.value(QStringLiteral("location")).toObject();
            const QString address = locationObject.value(QStringLiteral("address")).toString();
            const QString city = locationObject.value(QStringLiteral("city")).toString();
            const QString country = locationObject.value(QStringLiteral("country")).toString();
            const double latitude = locationObject.value(QStringLiteral("lat")).toString().toDouble();
            const double longitude = locationObject.value(QStringLiteral("lng")).toString().toDouble();
            const int usersCount = venueObject.value(QStringLiteral("stats")).toObject().value(QStringLiteral("usersCount")).toInt();

            const QJsonValue categoryIconValue = firstCategoryObject.value(QStringLiteral("icon"));
            QString iconUrl;
            QString largeIconUrl;
            if (categoryIconValue.isObject()) {
                const QJsonObject categoryIconObject = categoryIconValue.toObject();
                const QString iconPrefix = categoryIconObject.value(QStringLiteral("prefix")).toString();
                const QString iconName = categoryIconObject.value(QStringLiteral("name")).toString();
                iconUrl = iconPrefix + QLatin1StringView("32") // That's the icon size hardcoded
                    + iconName;

                largeIconUrl = iconPrefix + QLatin1StringView("64") // Larger icon
                    + iconName;
            }

            if (!itemExists(id)) {
                GeoDataCoordinates coordinates(longitude, latitude, 0.0, GeoDataCoordinates::Degree);
                FoursquareItem *item = new FoursquareItem(this);
                item->setId(id);
                item->setCoordinate(coordinates);
                item->setName(name);
                item->setCategory(category);
                item->setAddress(address);
                item->setCity(city);
                item->setCountry(country);
                item->setUsersCount(usersCount);
                item->setCategoryIconUrl(iconUrl);
                item->setCategoryLargeIconUrl(largeIconUrl);

                items << item;
            }
        }
        addItemsToList(items);
    }
}

}

#include "moc_FoursquareModel.cpp"
