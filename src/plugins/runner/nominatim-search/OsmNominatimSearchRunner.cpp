// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "OsmNominatimSearchRunner.h"

#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataPlacemark.h"
#include "HttpDownloadManager.h"
#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "StyleBuilder.h"
#include "osm/OsmPlacemarkData.h"

#include <QDomDocument>
#include <QList>
#include <QNetworkReply>
#include <QTimer>
#include <QUrl>

namespace Marble
{

OsmNominatimRunner::OsmNominatimRunner(QObject *parent)
    : SearchRunner(parent)
    , m_manager()
{
    connect(&m_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(handleResult(QNetworkReply *)));
}

OsmNominatimRunner::~OsmNominatimRunner()
{
    // nothing to do
}

void OsmNominatimRunner::returnNoResults()
{
    Q_EMIT searchFinished(QList<GeoDataPlacemark *>());
}

void OsmNominatimRunner::search(const QString &searchTerm, const GeoDataLatLonBox &preferred)
{
    QString base = "https://nominatim.openstreetmap.org/search?";
    QString query = "q=%1&format=xml&addressdetails=1&accept-language=%2";
    QString url = QString(base + query).arg(searchTerm).arg(MarbleLocale::languageCode());
    if (!preferred.isEmpty()) {
        GeoDataCoordinates::Unit deg = GeoDataCoordinates::Degree;
        QString viewbox("&viewbox=%1,%2,%3,%4&bounded=1"); // left, top, right, bottom
        url += viewbox.arg(preferred.west(deg)).arg(preferred.north(deg)).arg(preferred.east(deg)).arg(preferred.south(deg));
    }
    m_request.setUrl(QUrl(url));
    m_request.setRawHeader("User-Agent", HttpDownloadManager::userAgent("Browser", "OsmNominatimRunner"));

    QEventLoop eventLoop;

    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(15000);

    connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    connect(this, SIGNAL(searchFinished(QList<GeoDataPlacemark *>)), &eventLoop, SLOT(quit()));

    // @todo FIXME Must currently be done in the main thread, see bug 257376
    QTimer::singleShot(0, this, SLOT(startSearch()));
    timer.start();

    eventLoop.exec();
}

void OsmNominatimRunner::startSearch()
{
    QNetworkReply *reply = m_manager.get(m_request);
    connect(reply, &QNetworkReply::errorOccurred, this, &OsmNominatimRunner::returnNoResults);
}

GeoDataExtendedData OsmNominatimRunner::extractChildren(const QDomNode &node)
{
    GeoDataExtendedData data;
    QDomNodeList nodes = node.childNodes();
    for (int i = 0, n = nodes.length(); i < n; ++i) {
        QDomNode child = nodes.item(i);
        data.addValue(GeoDataData(child.nodeName(), child.toElement().text()));
    }
    return data;
}

void OsmNominatimRunner::handleResult(QNetworkReply *reply)
{
    QDomDocument xml;
    if (!xml.setContent(reply->readAll())) {
        qWarning() << "Cannot parse osm nominatim result";
        qWarning() << reply->error();
        returnNoResults();
        return;
    }

    QList<GeoDataPlacemark *> placemarks;
    QDomElement root = xml.documentElement();
    QDomNodeList places = root.elementsByTagName(QStringLiteral("place"));
    for (int i = 0; i < places.size(); ++i) {
        QDomNode place = places.at(i);
        QDomNamedNodeMap attributes = place.attributes();
        QString lon = attributes.namedItem(QStringLiteral("lon")).nodeValue();
        QString lat = attributes.namedItem(QStringLiteral("lat")).nodeValue();
        QString desc = attributes.namedItem(QStringLiteral("display_name")).nodeValue();
        QString key = attributes.namedItem(QStringLiteral("class")).nodeValue();
        QString value = attributes.namedItem(QStringLiteral("type")).nodeValue();

        OsmPlacemarkData data;

        GeoDataExtendedData placemarkData = extractChildren(place);
        placemarkData.addValue(GeoDataData(QStringLiteral("class"), key));
        placemarkData.addValue(GeoDataData(QStringLiteral("type"), value));

        QString name = place.firstChildElement(value).text();
        QString road = place.firstChildElement(QStringLiteral("road")).text();
        placemarkData.addValue(GeoDataData(QStringLiteral("name"), name));

        QString city = place.firstChildElement(QStringLiteral("city")).text();
        if (city.isEmpty()) {
            city = place.firstChildElement(QStringLiteral("town")).text();
            if (city.isEmpty()) {
                city = place.firstChildElement(QStringLiteral("village")).text();
            }
            if (city.isEmpty()) {
                city = place.firstChildElement(QStringLiteral("hamlet")).text();
            }
        }

        QString administrative = place.firstChildElement(QStringLiteral("county")).text();
        if (administrative.isEmpty()) {
            administrative = place.firstChildElement(QStringLiteral("region")).text();
            if (administrative.isEmpty()) {
                administrative = place.firstChildElement(QStringLiteral("state")).text();
                data.addTag(QStringLiteral("addr:state"), administrative);
            } else {
                data.addTag(QStringLiteral("district"), administrative);
            }
        }

        QString country = place.firstChildElement(QStringLiteral("country")).text();

        QString description;
        for (int i = 0; i < place.childNodes().size(); ++i) {
            QDomElement item = place.childNodes().at(i).toElement();
            description += item.nodeName() + QLatin1Char(':') + item.text() + QLatin1Char('\n');
        }
        description += QLatin1StringView("Category: ") + key + QLatin1Char('/') + value;

        if (!lon.isEmpty() && !lat.isEmpty() && !desc.isEmpty()) {
            QString placemarkName;
            GeoDataPlacemark *placemark = new GeoDataPlacemark;
            // try to provide 2 fields
            if (!name.isEmpty()) {
                placemarkName = name;
            }
            if (!road.isEmpty() && road != placemarkName) {
                if (!placemarkName.isEmpty()) {
                    placemarkName += QLatin1StringView(", ");
                }
                placemarkName += road;
                data.addTag(QStringLiteral("addr:street"), road);
            }
            if (!city.isEmpty() && !placemarkName.contains(QLatin1Char(',')) && city != placemarkName) {
                if (!placemarkName.isEmpty()) {
                    placemarkName += QLatin1StringView(", ");
                }
                placemarkName += city;
                data.addTag(QStringLiteral("addr:city"), city);
            }
            if (!administrative.isEmpty() && !placemarkName.contains(QLatin1Char(',')) && administrative != placemarkName) {
                if (!placemarkName.isEmpty()) {
                    placemarkName += QLatin1StringView(", ");
                }
                placemarkName += administrative;
            }
            if (!country.isEmpty() && !placemarkName.contains(QLatin1Char(',')) && country != placemarkName) {
                if (!placemarkName.isEmpty()) {
                    placemarkName += QLatin1StringView(", ");
                }
                placemarkName += country;
                data.addTag(QStringLiteral("addr:country"), country);
            }
            if (placemarkName.isEmpty()) {
                placemarkName = desc;
            }
            placemark->setName(placemarkName);
            placemark->setDescription(description);
            placemark->setAddress(desc);
            placemark->setCoordinate(lon.toDouble(), lat.toDouble(), 0, GeoDataCoordinates::Degree);
            const auto category = StyleBuilder::determineVisualCategory(data);
            placemark->setVisualCategory(category);
            placemark->setExtendedData(placemarkData);
            placemark->setOsmData(data);
            placemarks << placemark;
        }
    }

    Q_EMIT searchFinished(placemarks);
}

} // namespace Marble

#include "moc_OsmNominatimSearchRunner.cpp"
