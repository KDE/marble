// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "OsmNominatimReverseGeocodingRunner.h"

#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "GeoDataPlacemark.h"
#include "HttpDownloadManager.h"
#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "osm/OsmPlacemarkData.h"

#include <QDomDocument>
#include <QNetworkReply>
#include <QTimer>
#include <QUrl>

namespace Marble
{

OsmNominatimRunner::OsmNominatimRunner(QObject *parent)
    : ReverseGeocodingRunner(parent)
    , m_manager(this)
{
    connect(&m_manager, &QNetworkAccessManager::finished, this, &OsmNominatimRunner::handleResult);
}

OsmNominatimRunner::~OsmNominatimRunner()
{
    // nothing to do
}

void OsmNominatimRunner::returnNoReverseGeocodingResult()
{
    Q_EMIT reverseGeocodingFinished(m_coordinates, GeoDataPlacemark());
}

void OsmNominatimRunner::reverseGeocoding(const GeoDataCoordinates &coordinates)
{
    m_coordinates = coordinates;
    const QString base = QStringLiteral("https://nominatim.openstreetmap.org/reverse?format=xml&addressdetails=1");
    // @todo: Alternative URI with addressdetails=1 could be used for shorter placemark name
    const QString query = QStringLiteral("&lon=%1&lat=%2&accept-language=%3");
    double lon = coordinates.longitude(GeoDataCoordinates::Degree);
    double lat = coordinates.latitude(GeoDataCoordinates::Degree);
    QString url = QString(base + query).arg(lon).arg(lat).arg(MarbleLocale::languageCode());

    m_request.setUrl(QUrl(url));
    m_request.setRawHeader("User-Agent", HttpDownloadManager::userAgent(QStringLiteral("Browser"), QStringLiteral("OsmNominatimRunner")));

    QEventLoop eventLoop;

    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(15000);

    connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
    connect(this, &ReverseGeocodingRunner::reverseGeocodingFinished, &eventLoop, &QEventLoop::quit);

    // @todo FIXME Must currently be done in the main thread, see bug 257376
    QTimer::singleShot(0, this, SLOT(startReverseGeocoding()));
    timer.start();

    eventLoop.exec();
}

void OsmNominatimRunner::startReverseGeocoding()
{
    QNetworkReply *reply = m_manager.get(m_request);
    connect(reply, &QNetworkReply::errorOccurred, this, &OsmNominatimRunner::returnNoReverseGeocodingResult);
}

void OsmNominatimRunner::handleResult(QNetworkReply *reply)
{
    if (!reply->bytesAvailable()) {
        returnNoReverseGeocodingResult();
        return;
    }

    QDomDocument xml;
    if (!xml.setContent(reply->readAll())) {
        mDebug() << "Cannot parse osm nominatim result " << xml.toString();
        returnNoReverseGeocodingResult();
        return;
    }

    QDomElement root = xml.documentElement();
    QDomNodeList places = root.elementsByTagName(QStringLiteral("result"));
    if (places.size() == 1) {
        QString address = places.item(0).toElement().text();
        GeoDataPlacemark placemark;
        placemark.setVisualCategory(GeoDataPlacemark::Coordinate);
        placemark.setAddress(address);
        placemark.setCoordinate(m_coordinates);

        QDomNode details = root.firstChildElement(QStringLiteral("addressparts"));
        extractChildren(details, placemark);

        Q_EMIT reverseGeocodingFinished(m_coordinates, placemark);
    } else {
        returnNoReverseGeocodingResult();
    }
}

void OsmNominatimRunner::extractChildren(const QDomNode &node, GeoDataPlacemark &placemark)
{
    QMap<QString, QString> tagTranslator;
    tagTranslator[QStringLiteral("house_number")] = QStringLiteral("addr:housenumber");
    tagTranslator[QStringLiteral("road")] = QStringLiteral("addr:street");
    tagTranslator[QStringLiteral("suburb")] = QStringLiteral("addr:suburb");
    tagTranslator[QStringLiteral("city")] = QStringLiteral("addr:city");
    tagTranslator[QStringLiteral("state_district")] = QStringLiteral("addr:district");
    tagTranslator[QStringLiteral("state")] = QStringLiteral("addr:state");
    tagTranslator[QStringLiteral("postcode")] = QStringLiteral("addr:postcode");
    tagTranslator[QStringLiteral("country_code")] = QStringLiteral("addr:country"); // correct mapping
    // @todo Find a proper mapping for those
    // tagTranslator["village"] = "";
    // tagTranslator["town"] = "";

    GeoDataExtendedData data;
    OsmPlacemarkData osmData;
    QDomNodeList nodes = node.childNodes();
    for (int i = 0, n = nodes.length(); i < n; ++i) {
        QDomNode child = nodes.item(i);
        data.addValue(GeoDataData(child.nodeName(), child.toElement().text()));

        if (tagTranslator.contains(child.nodeName())) {
            QString const key = tagTranslator[child.nodeName()];
            osmData.addTag(key, child.toElement().text());
        }
    }
    placemark.setExtendedData(data);
    placemark.setOsmData(osmData);
}

} // namespace Marble

#include "moc_OsmNominatimReverseGeocodingRunner.cpp"
