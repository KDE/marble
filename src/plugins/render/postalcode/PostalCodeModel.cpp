// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Valery Kharitonov <kharvd@gmail.com>
//

// Self
#include "PostalCodeModel.h"

// Plugin
#include "PostalCodeItem.h"

// Marble
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleGlobal.h"
#include "MarbleModel.h"
#include "Planet.h"

// Qt
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QUrl>

#include <QUrlQuery>

using namespace Marble;

PostalCodeModel::PostalCodeModel(const MarbleModel *marbleModel, QObject *parent)
    : AbstractDataPluginModel(QStringLiteral("postalCode"), marbleModel, parent)
{
}

PostalCodeModel::~PostalCodeModel() = default;

void PostalCodeModel::getAdditionalItems(const GeoDataLatLonAltBox &box, qint32 number)
{
    if (marbleModel()->planetId() != QLatin1StringView("earth")) {
        return;
    }

    double const lat = box.center().latitude(GeoDataCoordinates::Degree);
    double const lon = box.center().longitude(GeoDataCoordinates::Degree);
    double const radius = qMin<double>(30.0, box.height() * marbleModel()->planet()->radius() * METER2KM);

    QUrl geonamesUrl(QStringLiteral("http://api.geonames.org/findNearbyPostalCodesJSON"));
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QStringLiteral("lat"), QString::number(lat));
    urlQuery.addQueryItem(QStringLiteral("lng"), QString::number(lon));
    urlQuery.addQueryItem(QStringLiteral("radius"), QString::number(radius));
    urlQuery.addQueryItem(QStringLiteral("maxRows"), QString::number(number));
    urlQuery.addQueryItem(QStringLiteral("username"), QStringLiteral("marble"));
    geonamesUrl.setQuery(urlQuery);

    downloadDescriptionFile(QUrl(geonamesUrl));
}

void PostalCodeModel::parseFile(const QByteArray &file)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file);
    QJsonValue postalCodesValue = jsonDoc.object().value(QStringLiteral("postalCodes"));

    // Parse if any result exists
    if (postalCodesValue.isArray()) {
        // Add items to the list
        QList<AbstractDataPluginItem *> items;

        QJsonArray postalCodesArray = postalCodesValue.toArray();
        for (int index = 0; index < postalCodesArray.size(); ++index) {
            QJsonObject postalCodeObject = postalCodesArray[index].toObject();

            QString const placeName = postalCodeObject.value(QStringLiteral("placeName")).toString();
            QString const adminName1 = postalCodeObject.value(QStringLiteral("adminName1")).toString();
            QString const adminName2 = postalCodeObject.value(QStringLiteral("adminName2")).toString();
            QString const adminName3 = postalCodeObject.value(QStringLiteral("adminName3")).toString();
            QString const postalCode = postalCodeObject.value(QStringLiteral("postalCode")).toString();
            QString const countryCode = postalCodeObject.value(QStringLiteral("countryCode")).toString();
            double const longitude = postalCodeObject.value(QStringLiteral("lng")).toDouble();
            double const latitude = postalCodeObject.value(QStringLiteral("lat")).toDouble();

            QString const id = QLatin1StringView("postalCode_") + countryCode + postalCode;

            if (!id.isEmpty()) {
                QString tooltip;

                if (!placeName.isEmpty()) {
                    tooltip += placeName + QLatin1Char(' ');
                }

                addLine(&tooltip, postalCode);
                addLine(&tooltip, countryCode);
                addLine(&tooltip, adminName1);
                addLine(&tooltip, adminName2);
                addLine(&tooltip, adminName3);
                tooltip = tooltip.trimmed();

                if (!itemExists(id)) {
                    // If it does not exist, create it
                    GeoDataCoordinates coordinates(longitude, latitude, 0.0, GeoDataCoordinates::Degree);

                    auto item = new PostalCodeItem(this);
                    item->setId(id);
                    item->setCoordinate(coordinates);
                    item->setToolTip(tooltip);
                    item->setText(postalCode);

                    items << item;
                }
            }
        }
        addItemsToList(items);
    }
}

void PostalCodeModel::addLine(QString *string, const QString &line)
{
    Q_ASSERT(string);
    if (!line.isEmpty()) {
        *string += line + QLatin1Char('\n');
    }
}

#include "moc_PostalCodeModel.cpp"
