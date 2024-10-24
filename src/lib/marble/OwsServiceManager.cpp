// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2023 Torsten Rahn <rahn@kde.org>
//

#include "OwsServiceManager.h"

#include <QAuthenticator>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

#include <QBuffer>
#include <QImageReader>

#include <MarbleDebug.h>
#include <cmath>

namespace Marble
{

OwsMappingCapabilities::OwsMappingCapabilities() = default;

WmsCapabilities::WmsCapabilities() = default;

WmtsCapabilities::WmtsCapabilities() = default;

void WmtsCapabilities::setWmtsTileMatrixSets(const QMap<QString, QStringList> &wmtsTileMatrixSets)
{
    m_wmtsTileMatrixSets = wmtsTileMatrixSets;
}

QMap<QString, QStringList> WmtsCapabilities::wmtsTileMatrixSets() const
{
    return m_wmtsTileMatrixSets;
}

void WmtsCapabilities::setWmtsTileResource(const QMap<QString, QMap<QString, QString>> &wmtsTileRessource)
{
    m_wmtsTileResource = wmtsTileRessource;
}

QMap<QString, QMap<QString, QString>> WmtsCapabilities::wmtsTileResource() const
{
    return m_wmtsTileResource;
}

ImageRequestResult::ImageRequestResult()
    : m_imageStatus(WmsImageNone)
{
}

void ImageRequestResult::setImageStatus(WmsImageStatus imageStatus)
{
    m_imageStatus = imageStatus;
}

WmsImageStatus ImageRequestResult::imageStatus() const
{
    return m_imageStatus;
}

void ImageRequestResult::setResultImage(const QImage &image)
{
    m_resultImage = image;
}

QImage ImageRequestResult::resultImage() const
{
    return m_resultImage;
}

void ImageRequestResult::setResultRaw(const QByteArray &resultRaw)
{
    m_resultRaw = resultRaw;
}

QByteArray ImageRequestResult::resultRaw() const
{
    return m_resultRaw;
}

void ImageRequestResult::setResultFormat(const QString &resultFormat)
{
    m_resultFormat = resultFormat;
}

QString ImageRequestResult::resultFormat() const
{
    return m_resultFormat;
}

void ImageRequestResult::setResultType(const ImageResultType resultType)
{
    m_resultType = resultType;
}

ImageResultType ImageRequestResult::resultType() const
{
    return m_resultType;
}

void OwsMappingCapabilities::setVersion(const QString &version)
{
    m_version = version;
}

QString OwsMappingCapabilities::version() const
{
    return m_version;
}

void OwsMappingCapabilities::setTitle(const QString &title)
{
    m_title = title;
}

QString OwsMappingCapabilities::title() const
{
    return m_title;
}

void OwsMappingCapabilities::setAbstract(const QString &abstract)
{
    m_abstract = abstract;
}

QString OwsMappingCapabilities::abstract() const
{
    return m_abstract;
}

void WmsCapabilities::setContactInformation(const QString &info)
{
    m_contactInformation = info;
}

QString WmsCapabilities::contactInformation() const
{
    return m_contactInformation;
}

void WmsCapabilities::setFees(const QString &fees)
{
    m_fees = fees;
}

QString WmsCapabilities::fees() const
{
    return m_fees;
}

QStringList OwsMappingCapabilities::layers() const
{
    return m_owsLayerMetaInfo.keys();
}

QStringList WmsCapabilities::projections(const QString &layer)
{
    QStringList result = m_wmsLayerCoordinateSystems.value(layer).keys();
    if (!m_wmsLayerCoordinateSystems.isEmpty()) {
        result << m_wmsLayerCoordinateSystems.value(m_wmsLayerCoordinateSystems.firstKey()).keys();
    }
    result.removeDuplicates();

    return result;
}

QString WmsCapabilities::boundingBox(const QString &layer, const QString &projection)
{
    QString result;
    result = m_wmsLayerCoordinateSystems.value(layer).value(projection);
    if (result.isEmpty()) {
        result = m_wmsLayerCoordinateSystems.value(nullptr).value(projection);
    }
    return result;
}

QString OwsMappingCapabilities::title(const QString &layer)
{
    return m_owsLayerMetaInfo.value(layer).at(0);
}

QString OwsMappingCapabilities::abstract(const QString &layer)
{
    return m_owsLayerMetaInfo.value(layer).at(1);
}

QString WmsCapabilities::legendUrl(const QString &layer)
{
    return m_owsLayerMetaInfo.value(layer).at(2);
}

QString OwsMappingCapabilities::style(const QString &layer)
{
    return m_owsLayerMetaInfo.value(layer).at(3);
}

QStringList WmsCapabilities::styles(const QStringList &layers)
{
    QStringList retVal;
    for (auto layer : layers) {
        retVal << style(layer);
    }
    return retVal;
}

QString WmsCapabilities::boundingBoxNSEWDegrees(const QStringList &layers, const QString &projection)
{
    QString retVal;
    for (auto layer : layers) {
        QString layerBBox = boundingBox(layer, projection);
        if (layerBBox.isEmpty()) {
            retVal = QStringLiteral("90,-90,180,-180");
            break;
        }
        QStringList layerBBoxList = layerBBox.split(",");
        qreal west, south, east, north;
        if (projection == QStringLiteral("epsg:3857")) {
            west = layerBBoxList.at(0).toDouble() * 180 / 20037508.34;
            south = atan(pow(2.7182818284, (layerBBoxList.at(1).toDouble() / 20037508.34 * M_PI))) * (360 / M_PI) - 90;
            east = layerBBoxList.at(2).toDouble() * 180 / 20037508.34;
            north = atan(pow(2.7182818284, (layerBBoxList.at(3).toDouble() / 20037508.34 * M_PI))) * (360 / M_PI) - 90;
        } else {
            if (projection == "crs:84" || (projection == "4326" && version() != "1.3.0")) {
                // order: longitude-latitude
                west = layerBBoxList.at(0).toDouble();
                south = layerBBoxList.at(1).toDouble();
                east = layerBBoxList.at(2).toDouble();
                north = layerBBoxList.at(3).toDouble();
            } else {
                // order: latitude-longitude
                west = layerBBoxList.at(1).toDouble();
                south = layerBBoxList.at(0).toDouble();
                east = layerBBoxList.at(3).toDouble();
                north = layerBBoxList.at(2).toDouble();
            }
        }
        retVal = QStringLiteral("%1,%2,%3,%4").arg(north).arg(south).arg(east).arg(west);
        // TODO: merge possibly different layer bboxes
        break;
    }
    return retVal;
}

void WmsCapabilities::setReferenceSystemType(const QString &refSystem)
{
    m_referenceSystemType = refSystem;
}

QString WmsCapabilities::referenceSystemType() const
{
    return m_referenceSystemType;
}

void OwsMappingCapabilities::setOwsLayerMetaInfo(const QMap<QString, QStringList> &wmsLayerMetaInfo)
{
    m_owsLayerMetaInfo = wmsLayerMetaInfo;
}

QMap<QString, QStringList> OwsMappingCapabilities::owsLayerMetaInfo() const
{
    return m_owsLayerMetaInfo;
}

void WmsCapabilities::setWmsLayerCoordinateSystems(const QMap<QString, QMap<QString, QString>> &wmsLayerCoordinateSystems)
{
    m_wmsLayerCoordinateSystems = wmsLayerCoordinateSystems;
}

QMap<QString, QMap<QString, QString>> WmsCapabilities::wmsLayerCoordinateSystems() const
{
    return m_wmsLayerCoordinateSystems;
}

void WmsCapabilities::setFormats(const QStringList &formats)
{
    m_formats = formats;
}

QStringList WmsCapabilities::formats()
{
    return m_formats;
}

OwsServiceManager::OwsServiceManager(QObject *parent)
    : QObject(parent)
    , m_capabilitiesStatus(OwsCapabilitiesNone)
{
    connect(&m_capabilitiesAccessManager, &QNetworkAccessManager::finished, this, &OwsServiceManager::parseOwsCapabilities);
    connect(&m_imageAccessManager, &QNetworkAccessManager::finished, this, &OwsServiceManager::parseImageResult);

    connect(&m_capabilitiesAccessManager, &QNetworkAccessManager::authenticationRequired, this, &OwsServiceManager::handleAuthentication);
    connect(&m_imageAccessManager, &QNetworkAccessManager::authenticationRequired, this, &OwsServiceManager::handleAuthentication);
}

void OwsServiceManager::queryOwsCapabilities(const QUrl &queryUrl, const QString &serviceString)
{
    m_url = queryUrl;
    QUrl url(queryUrl);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QStringLiteral("service"), serviceString);
    urlQuery.addQueryItem(QStringLiteral("request"), QStringLiteral("GetCapabilities"));
    url.setQuery(urlQuery);

    QNetworkRequest request;
    request.setUrl(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    mDebug() << "for url" << url;
    m_capabilitiesAccessManager.get(request);
}

void OwsServiceManager::queryWmsMap(const QUrl &url,
                                    const QString &layers,
                                    const QString &projection,
                                    const QString &bbox,
                                    const QString &format,
                                    const QString &style)
{
    m_imageRequestResult.setResultImage(QImage());
    m_imageRequestResult.setImageStatus(WmsImageNone);
    m_imageRequestResult.setResultRaw("");
    m_imageRequestResult.setResultFormat(QString());

    QUrlQuery downloadQuery;
    downloadQuery.addQueryItem("request", "GetMap"); // Requests that the server generates a map.
    downloadQuery.addQueryItem("service", "wms"); // Service name. Value is WMS.

    QString versionkey = wmsCapabilities().version() == "1.0.0" ? "wmtver" : "version";
    downloadQuery.addQueryItem(versionkey, wmsCapabilities().version()); // Service version. Value is one of 1.0.0, 1.1.0, 1.1.1, 1.3.0.
    downloadQuery.addQueryItem("layers", layers); // Layers to display on map. Value is a comma-separated list of layer names.

    // Spatial Reference System for map output. Value is in the form EPSG:nnn. srs was used before WMS 1.3.0, crs has been used since then.
    downloadQuery.addQueryItem(wmsCapabilities().referenceSystemType(), projection);
    // downloadQuery.addQueryItem( "bgcolor", "#ff0000" ); // rarely supported by servers

    downloadQuery.addQueryItem("width", "256"); // Width of map output, in pixels.
    downloadQuery.addQueryItem("height", "256"); // Height of map output, in pixels.

    QString boundingBox = bbox;
    if (boundingBox.isEmpty()) {
        if (projection == "epsg:3857") {
            boundingBox = "-20037508.34,-20048966.1,20037508.34,20048966.1";
        } else if (projection == "epsg:4326") {
            boundingBox =
                wmsCapabilities().version() == "1.3.0" ? "-90,-180,90,180" : "-180,-90,180,90"; // flipped axes for 1.3.0 in epsg:4326 according to spec
        } else if (projection == "crs:84") {
            boundingBox = "-180,-90,180,90"; // order: WGS84 longitude-latitude
        }
    }
    downloadQuery.addQueryItem("bbox", boundingBox);
    downloadQuery.addQueryItem("transparent", "true");

    // Format for the map output. In addition to common bitmap formats WMS servers
    // sometimes support "vector" formats (PDF, SVG, KML, etc.)
    // Currently Marble only supports JPEG, PNG, TIFF, GIF, BMP and their variants.
    downloadQuery.addQueryItem("format", QStringLiteral("image/%1").arg(format));

    // Styles in which layers are to be rendered. Value is a comma-separated list of style names,
    // or empty if default styling is required. Style names may be empty in the list,
    // to use default layer styling. However some servers do not accept empty style names.
    downloadQuery.addQueryItem("styles", style);
    m_imageRequestResult.setResultFormat((format == QLatin1StringView("jpeg")) ? "jpg" : format); // Is this needed here?

    QUrl finalDownloadUrl(url);
    finalDownloadUrl.setQuery(downloadQuery);
    mDebug() << "requesting WMS image" << finalDownloadUrl;

    QNetworkRequest request(finalDownloadUrl);

    m_imageAccessManager.get(request);
}

void OwsServiceManager::queryWmsLevelZeroTile(const QUrl &url, const QString &layers, const QString &projection, const QString &format, const QString &style)
{
    QString bbox;
    if (projection == QStringLiteral("epsg:3857")) {
        bbox = QStringLiteral("-20037508.34,-20048966.1,20037508.34,20048966.1");

    } else if (projection == QStringLiteral("epsg:4326")) {
        bbox = wmsCapabilities().version() == QStringLiteral("1.3.0")
            ? QStringLiteral("-90,-180,90,180")
            : QStringLiteral("-180,-90,180,90"); // flipped axes for 1.3.0 in epsg:4326 according to spec
    } else if (projection == QStringLiteral("crs:84")) {
        bbox = QStringLiteral("-180,-90,180,90"); // order: WGS84 longitude-latitude
    }

    m_imageRequestResult.setResultType(LevelZeroTile);

    queryWmsMap(url, layers, projection, bbox, format, style);
}

void OwsServiceManager::queryWmsPreviewImage(const QUrl &url, const QString &layers, const QString &projection, const QString &format, const QString &style)
{
    QString firstLayer = layers.contains(',') ? layers.section(',', 0, 0) : layers;
    QString bbox = wmsCapabilities().boundingBox(firstLayer, projection);

    m_imageRequestResult.setResultType(PreviewImage);

    queryWmsMap(url, layers, projection, bbox, format, style);
}

void OwsServiceManager::queryWmsLegendImage(const QUrl &url)
{
    m_imageRequestResult.setResultImage(QImage());
    m_imageRequestResult.setImageStatus(WmsImageNone);
    m_imageRequestResult.setResultRaw("");
    m_imageRequestResult.setResultFormat(QString());
    m_imageRequestResult.setResultType(LegendImage);

    mDebug() << "requesting legend" << url;

    QNetworkRequest request(url);

    m_imageAccessManager.get(request);
}

void OwsServiceManager::queryWmtsLevelZeroTile(const QString &url, const QString &style, const QString &tileMatrixSet)
{
    m_imageRequestResult.setResultType(LevelZeroTile);
    queryWmtsTile(url, style, tileMatrixSet, "0", "0", QStringLiteral("0"));
}

void OwsServiceManager::queryWmtsPreviewImage(const QString &url, const QString &style, const QString &tileMatrixSet)
{
    m_imageRequestResult.setResultType(PreviewImage);
    queryWmtsTile(url, style, tileMatrixSet, "0", "0", QStringLiteral("0"));
}

void OwsServiceManager::queryWmtsTile(const QString &url,
                                      const QString &style,
                                      const QString &tileMatrixSet,
                                      const QString &tileMatrix,
                                      const QString &tileRow,
                                      const QString &tileCol)
{
    m_imageRequestResult.setResultImage(QImage());
    m_imageRequestResult.setImageStatus(WmsImageNone);
    m_imageRequestResult.setResultRaw("");
    m_imageRequestResult.setResultFormat(QString());

    QUrl downloadUrl;
    QString baseUrl = url;
    baseUrl.replace(baseUrl.indexOf(QLatin1StringView("{Time}")), 6, QStringLiteral("current"));
    baseUrl.replace(baseUrl.indexOf(QLatin1StringView("{style}")), 7, style);
    baseUrl.replace(baseUrl.indexOf(QLatin1StringView("{Style}")), 7, style);
    baseUrl.replace(baseUrl.indexOf(QLatin1StringView("{TileMatrixSet}")), 15, tileMatrixSet);
    baseUrl.replace(baseUrl.indexOf(QLatin1StringView("{TileMatrix}")), 12, tileMatrix);
    baseUrl.replace(baseUrl.indexOf(QLatin1StringView("{TileRow}")), 9, tileRow);
    baseUrl.replace(baseUrl.indexOf(QLatin1StringView("{TileCol}")), 9, tileCol);
    downloadUrl.setUrl(baseUrl);

    QNetworkRequest request(downloadUrl);
    mDebug() << "requesting static map" << downloadUrl;
    m_imageAccessManager.get(request);
}

void OwsServiceManager::queryXYZPreviewImage(const QString &urlString)
{
    m_imageRequestResult.setResultType(PreviewImage);
    queryXYZImage(urlString);
}

void OwsServiceManager::queryXYZLevelZeroTile(const QString &urlString)
{
    m_imageRequestResult.setResultType(LevelZeroTile);
    queryXYZImage(urlString);
}

void OwsServiceManager::queryXYZImage(const QString urlString)
{
    QUrl downloadUrl;
    QString baseUrl = urlString;
    baseUrl.replace(baseUrl.indexOf(QLatin1StringView("{x}")), 3, QString::number(0));
    baseUrl.replace(baseUrl.indexOf(QLatin1StringView("{y}")), 3, QString::number(0));
    baseUrl.replace(baseUrl.indexOf(QLatin1StringView("{zoomLevel}")), 11, QString::number(0));
    baseUrl.replace(baseUrl.indexOf(QLatin1StringView("{z}")), 3, QString::number(0));
    downloadUrl.setUrl(baseUrl);

    QNetworkRequest request(downloadUrl);
    mDebug() << "requesting static map" << downloadUrl;
    m_imageAccessManager.get(request);
}

void OwsServiceManager::handleAuthentication(QNetworkReply *reply, QAuthenticator *authenticator)
{
    if (reply->url().host() == QStringLiteral("api.tileserver.org")) {
        authenticator->setUser({});
        authenticator->setPassword({});
    }
}

void OwsServiceManager::setCapabilitiesStatus(OwsCapabilitiesStatus capabilitiesStatus)
{
    m_capabilitiesStatus = capabilitiesStatus;
}

OwsCapabilitiesStatus OwsServiceManager::capabilitiesStatus() const
{
    return m_capabilitiesStatus;
}

OwsServiceType OwsServiceManager::owsServiceType()
{
    return m_owsServiceType;
}

WmsCapabilities Marble::OwsServiceManager::wmsCapabilities()
{
    return m_wmsCapabilities;
}

WmtsCapabilities OwsServiceManager::wmtsCapabilities()
{
    return m_wmtsCapabilities;
}

ImageRequestResult OwsServiceManager::imageRequestResult()
{
    return m_imageRequestResult;
}

QImage OwsServiceManager::resultImage()
{
    return m_imageRequestResult.resultImage();
}

ImageResultType OwsServiceManager::resultType()
{
    return m_imageRequestResult.resultType();
}

QByteArray OwsServiceManager::resultRaw()
{
    return m_imageRequestResult.resultRaw();
}

QString OwsServiceManager::resultFormat()
{
    return m_imageRequestResult.resultFormat();
}

void OwsServiceManager::parseOwsCapabilities(QNetworkReply *reply)
{
    mDebug() << "received reply from" << reply->url();
    QString result(reply->readAll());

    m_wmsCapabilities = WmsCapabilities(); // clear()

    if (!m_xml.setContent(result)) {
        setCapabilitiesStatus(OwsCapabilitiesReplyUnreadable); // Wizard cannot parse server's response
        Q_EMIT wmsCapabilitiesReady();
        return;
    }

    if (m_xml.documentElement().firstChildElement().tagName().isNull() || !m_xml.documentElement().tagName().contains("Capabilities")) {
        setCapabilitiesStatus(OwsCapabilitiesNoOwsServer); // Server is not a Ows Server.
        Q_EMIT wmsCapabilitiesReady();
        return;
    }

    if (m_xml.documentElement().tagName() == "WMS_Capabilities" || m_xml.documentElement().tagName() == "WMT_MS_Capabilities") { // WMTS server used for WMS?
        m_owsServiceType = WmsType;
        parseWmsCapabilities(reply);
    } else if (m_xml.documentElement().tagName() == "Capabilities") {
        m_owsServiceType = WmtsType;
        parseWmtsCapabilities(reply);
    } else if (m_xml.documentElement().tagName() == ("wfs:WFS_Capabilities")) {
        m_owsServiceType = WfsType;
    } else if (m_xml.documentElement().tagName() == ("wcs:Capabilities")) {
        m_owsServiceType = WcsType;
    } else {
        m_owsServiceType = NoOwsType;
    }
}

void OwsServiceManager::parseWmsCapabilities(QNetworkReply *reply)
{
    Q_UNUSED(reply)

    m_wmsCapabilities.setVersion(m_xml.documentElement().attribute("version"));
    m_wmsCapabilities.setReferenceSystemType(
        (m_wmsCapabilities.version() == "1.0.0" || m_wmsCapabilities.version() == "1.1.0" || m_wmsCapabilities.version() == "1.1.1") ? "SRS" : "CRS");

    QDomElement service = m_xml.documentElement().firstChildElement("Service");
    QDomNodeList layers = m_xml.documentElement().firstChildElement("Capability").elementsByTagName("Layer");

    m_wmsCapabilities.setTitle(service.firstChildElement("Title").text());
    m_wmsCapabilities.setAbstract(service.firstChildElement("Abstract").text());

    QDomElement contactElement = service.firstChildElement("ContactInformation");
    QDomElement contactPersonPrimaryElement = contactElement.firstChildElement("ContactPersonPrimary");
    QString contactPersonPrimary;
    if (!contactPersonPrimaryElement.isNull()) {
        QString contactPerson = contactPersonPrimaryElement.firstChildElement("ContactPerson").text();
        QString contactOrganisation = contactPersonPrimaryElement.firstChildElement("ContactOrganization").text();
        contactPersonPrimary = contactPerson + "<br>" + contactOrganisation + "<br>";
    }
    QString contactPosition = contactElement.firstChildElement("ContactPosition").text();
    contactPersonPrimary += contactPosition;

    QDomElement addressElement = contactElement.firstChildElement("ContactAddress");
    QString postalAddress;
    if (!addressElement.isNull() && addressElement.firstChildElement("AddressType").text() == "postal") {
        QString address = addressElement.firstChildElement("Address").text();
        QString city = addressElement.firstChildElement("City").text();
        QString stateOrProvince = addressElement.firstChildElement("StateOrProvince").text();
        QString postalCode = addressElement.firstChildElement("PostCode").text();
        QString country = addressElement.firstChildElement("Country").text();
        postalAddress = address + "<br>" + city + "<br>" + stateOrProvince + "<br>" + postalCode + "<br>" + country;
    }
    QString contactVoicePhone = contactElement.firstChildElement("ContactVoiceTelephone").text();
    QString contactFacsimileTelephone = contactElement.firstChildElement("ContactFacsimileTelephone").text();
    QString contactEmail = contactElement.firstChildElement("ContactElectronicMailAddress").text();
    QString contactMedium = contactVoicePhone + "<br>" + contactFacsimileTelephone + "<br>" + contactEmail;

    QString contactInformation = contactPersonPrimary + "<br><small><font color=\"darkgrey\">" + postalAddress + "<br>" + contactMedium + "</font></small>";

    m_wmsCapabilities.setContactInformation(contactInformation);
    QString fees = service.firstChildElement("Fees").text();
    m_wmsCapabilities.setFees(fees);

    QMap<QString, QStringList> wmsLayerMetaInfo;

    for (int i = 0; i < layers.size(); ++i) {
        QString name = layers.at(i).firstChildElement("Name").text();
        QString title = layers.at(i).firstChildElement("Title").text();
        QString abstract = layers.at(i).firstChildElement("Abstract").text();
        QDomElement legendElement = layers.at(i).firstChildElement("Style").firstChildElement("LegendURL");
        QString legendUrl;
        if (!legendElement.isNull())
            legendUrl = legendElement.firstChildElement("OnlineResource").attribute("xlink:href");
        QString style = layers.at(i).firstChildElement("Style").firstChildElement("Name").text();
        if (style.isEmpty())
            style = "default";
        /*      QDomElement gbboxElement = layers.at(i).firstChildElement("EX_GeographicBoundingBox");
                QStringList bbox;
                if (!gbboxElement.isNull()) {
                    bbox << gbboxElement.firstChildElement("westBoundLongitude").text() << gbboxElement.firstChildElement("southBoundLatitude").text()
                         << gbboxElement.firstChildElement("eastBoundLongitude").text() << gbboxElement.firstChildElement("northBoundLatitude").text();
                }
                wmsLayerMetaInfo[ name ] << title << abstract << legendUrl << style << bbox.join(","); */
        wmsLayerMetaInfo[name] << title << abstract << legendUrl << style;
    }

    m_wmsCapabilities.setOwsLayerMetaInfo(wmsLayerMetaInfo);

    QMap<QString, QMap<QString, QString>> wmsLayerCoordinateSystems;
    for (int i = 0; i < layers.size(); ++i) {
        QString layerName = layers.at(i).firstChildElement("Name").text();
        QDomNodeList projectionList = layers.at(i).toElement().elementsByTagName(m_wmsCapabilities.referenceSystemType());
        QDomNodeList layerPreviewBBox = layers.at(i).toElement().elementsByTagName("BoundingBox");

        for (int s = 0; s < projectionList.size(); ++s) {
            QString projection = projectionList.at(s).toElement().text().toLower();
            // SRS and CRS tags might contain a list of epsgs, so we need to use contains()
            if (projection.contains("epsg:3857")) {
                /*              if (wmsLayerMetaInfo.value(layerName).at(4) != ",,,") {  // EX_GeographicBoundingBox
                                    QStringList coords = wmsLayerMetaInfo.value(layerName).at(4).split(",");
                                    double west = (coords.at(0).toDouble() * 20037508.34) / 180;
                                    double south = 20037508.34 / M_PI * log(tan(((90 + coords.at(1).toDouble()) * M_PI) / 360));
                                    double east = (coords.at(2).toDouble() * 20037508.34) / 180;
                                    double north = 20037508.34 / M_PI * log(tan(((90 + coords.at(3).toDouble()) * M_PI) / 360));
                                    QString bbox = QStringLiteral("%1,%2,%3,%4").arg(QString::number( west, 'f', 6 )).arg(QString::number( south, 'f', 6 ))
                                                                         .arg(QString::number( east, 'f', 6 )).arg(QString::number( north, 'f', 6 ));
                                    wmsLayerCoordinateSystems[layerName]["epsg:3857"] = bbox;
                                }
                                else */
                wmsLayerCoordinateSystems[layerName]["epsg:3857"] = QString();
            }
            if (projection.contains("epsg:4326")) {
                /*              if (wmsLayerMetaInfo.value(layerName).at(4) != ",,,") {
                                    wmsLayerCoordinateSystems[layerName]["epsg:4326"] = wmsLayerMetaInfo.value(layerName).at(4); // Ignores flip
                                }
                                else */
                wmsLayerCoordinateSystems[layerName]["epsg:4326"] = QString();
            }
            if (projection.contains("crs:84")) {
                wmsLayerCoordinateSystems[layerName]["crs:84"] = QString();
            }
        }
        for (int b = 0; b < layerPreviewBBox.size(); ++b) {
            QDomElement bboxElement = layerPreviewBBox.at(b).toElement();
            QString bboxProjection = bboxElement.attribute(m_wmsCapabilities.referenceSystemType());
            if (bboxProjection != "epsg:3857" && bboxProjection != "epsg:4326" && bboxProjection != "crs:84")
                continue;
            int precision = bboxProjection == "epsg:3857" ? 6 : 12;
            double west = bboxElement.attribute("minx").toDouble();
            double south = bboxElement.attribute("miny").toDouble();
            double east = bboxElement.attribute("maxx").toDouble();
            double north = bboxElement.attribute("maxy").toDouble();
            QString bboxString = QStringLiteral("%1,%2,%3,%4")
                                     .arg(QString::number(west, 'f', precision),
                                          QString::number(south, 'f', precision),
                                          QString::number(east, 'f', precision),
                                          QString::number(north, 'f', precision));
            //          TODO: convert bbox coordinates from UTM to 3857 (e.g. from epsg:25832/33)
            wmsLayerCoordinateSystems[layerName][bboxProjection] = bboxString;
        }
        // FIXME: parse EX_GeographicBoundingBox if wmsLayerCoordinateSystems[layerName]["epsg:4326"/"epsg:3857"] == QString()
    }

    m_wmsCapabilities.setWmsLayerCoordinateSystems(wmsLayerCoordinateSystems);

    QDomNodeList formatList =
        m_xml.documentElement().firstChildElement("Capability").firstChildElement("Request").firstChildElement("GetMap").elementsByTagName("Format");

    QStringList formats;
    for (int f = 0; f < formatList.size(); ++f) {
        QString format = formatList.at(f).toElement().text();
        format = format.right(format.length() - format.indexOf(QLatin1Char('/')) - 1).toLower();
        if (format == "jpeg" || format.contains("png") || format.contains("tif") || format.contains("gif") || format.contains("bmp")
            || format.contains("jpg")) {
            formats << format;
        }
    }

    m_wmsCapabilities.setFormats(formats);

    setCapabilitiesStatus(OwsCapabilitiesSuccess);
    Q_EMIT wmsCapabilitiesReady();
}

void OwsServiceManager::parseWmtsCapabilities(QNetworkReply *reply)
{
    Q_UNUSED(reply)

    m_wmsCapabilities.setVersion(m_xml.documentElement().firstChildElement("ows:ServiceIdentification").firstChildElement("ows:ServiceTypeVersion").text());

    QDomElement service = m_xml.documentElement().firstChildElement("ows:ServiceIdentification");
    QDomNodeList layers = m_xml.documentElement().firstChildElement("Contents").elementsByTagName("Layer");

    m_wmtsCapabilities.setTitle(service.firstChildElement("ows:Title").text());
    m_wmtsCapabilities.setAbstract(service.firstChildElement("ows:Abstract").text());

    QMap<QString, QStringList> wmtsLayerMetaInfo;
    QMap<QString, QStringList> wmtsTileMatrixSets;
    QMap<QString, QMap<QString, QString>> wmtsTileResource;

    for (int i = 0; i < layers.size(); ++i) {
        QString name = layers.at(i).firstChildElement("ows:Identifier").text();
        QString title = layers.at(i).firstChildElement("ows:Title").text();
        QString abstract = layers.at(i).firstChildElement("ows:Abstract").text();
        QDomElement legendElement = layers.at(i).firstChildElement("Style").firstChildElement("LegendURL");
        QString legendUrl;
        if (!legendElement.isNull())
            legendUrl = legendElement.attribute("xlink:href");
        QString style = layers.at(i).firstChildElement("Style").firstChildElement("ows:Identifier").text();

        wmtsLayerMetaInfo[name] << title << abstract << legendUrl << style;

        QDomNodeList resourceList = layers.at(i).toElement().elementsByTagName("ResourceURL");
        for (int r = 0; r < resourceList.size(); ++r) {
            if (resourceList.at(r).toElement().attribute("resourceType") == "tile") {
                QString format = resourceList.at(r).toElement().attribute("format");
                QString resultFormat;
                format = format.right(format.length() - format.indexOf(QLatin1Char('/')) - 1).toLower();
                if (format == "jpeg" || format.contains("png") || format.contains("tif") || format.contains("gif") || format.contains("bmp")
                    || format.contains("jpg")) {
                    resultFormat = format;
                } else {
                    continue;
                }
                QString templ = resourceList.at(r).toElement().attribute(QStringLiteral("template"));
                wmtsTileResource[name][resultFormat] = templ;
            }
        }

        QStringList tileMatrixSets;
        QDomNodeList tileMatrixLinkList = layers.at(i).toElement().elementsByTagName(QStringLiteral("TileMatrixSetLink"));
        for (int t = 0; t < tileMatrixLinkList.size(); ++t) {
            tileMatrixSets << tileMatrixLinkList.at(t).toElement().firstChildElement(QStringLiteral("TileMatrixSet")).text();
        }
        wmtsTileMatrixSets[name] = tileMatrixSets;
    }

    m_wmtsCapabilities.setWmtsTileMatrixSets(wmtsTileMatrixSets);
    m_wmtsCapabilities.setWmtsTileResource(wmtsTileResource);

    m_wmtsCapabilities.setOwsLayerMetaInfo(wmtsLayerMetaInfo);
    setCapabilitiesStatus(OwsCapabilitiesSuccess);
    Q_EMIT wmtsCapabilitiesReady();
}

void OwsServiceManager::parseImageResult(QNetworkReply *reply)
{
    // QString request = reply->request().url().toString();

    // m_imageResult is already reset in the queryWmsMap

    QImage testImage;
    m_imageRequestResult.setResultRaw(reply->readAll());
    testImage = QImage::fromData(m_imageRequestResult.resultRaw());

    if (testImage.isNull()) {
        m_imageRequestResult.setImageStatus(WmsImageFailedServerMessage); // Image could not be downloaded
    } else if (m_imageRequestResult.resultRaw().isNull()) {
        m_imageRequestResult.setImageStatus(WmsImageFailed); // Image could not be downloaded
    } else {
        m_imageRequestResult.setImageStatus(WmsImageSuccess);
    }

    m_imageRequestResult.setResultImage(testImage);

    QByteArray resultRaw = m_imageRequestResult.resultRaw();
    QBuffer testBuffer(&resultRaw);
    m_imageRequestResult.setResultFormat(QImageReader(&testBuffer).format());

    Q_EMIT imageRequestResultReady();
}

}

#include "moc_OwsServiceManager.cpp"
