// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2023 Torsten Rahn <rahn@kde.org>
//

#include "OwsServiceManager.h"

#include <QUrl>
#include <QUrlQuery>
#include <QNetworkReply>

#include <QDomDocument>
#include <QBuffer>
#include <QImageReader>

#include <MarbleDebug.h>


namespace Marble
{

WmsCapabilities::WmsCapabilities()
    : m_capabilitiesStatus(WmsCapabilitiesNone)
{
}

void WmsCapabilities::setCapabilitiesStatus(WmsCapabilitiesStatus capabilitiesStatus)
{
    m_capabilitiesStatus = capabilitiesStatus;
}

WmsCapabilitiesStatus WmsCapabilities::capabilitiesStatus() const
{
    return m_capabilitiesStatus;
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

void WmsCapabilities::setVersion(const QString &version)
{
    m_version = version;
}

QString WmsCapabilities::version() const
{
    return m_version;
}

void WmsCapabilities::setTitle(const QString &title)
{
    m_title = title;
}

QString WmsCapabilities::title() const
{
    return m_title;
}

void WmsCapabilities::setAbstract(const QString &abstract)
{
    m_abstract = abstract;
}

QString WmsCapabilities::abstract() const
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

QStringList WmsCapabilities::layers() const
{
    return m_wmsLayerMetaInfo.keys();
}

QStringList WmsCapabilities::projections(const QString &layer)
{
    QStringList result = m_wmsLayerCoordinateSystems.value(layer).keys() << m_coordinateSystems.keys();

    result.removeDuplicates();

    return result;
}

QString WmsCapabilities::boundingBox(const QString &layer, const QString &projection)
{
    QString result;
    result = m_wmsLayerCoordinateSystems.value(layer).value(projection);
    if (result.isEmpty()) {
        result = m_wmsLayerCoordinateSystems.value(0).value(projection);
    }
    return result;
}

QString WmsCapabilities::title(const QString &layer)
{
    return m_wmsLayerMetaInfo.value(layer).at(0);
}

QString WmsCapabilities::abstract(const QString &layer)
{
    return m_wmsLayerMetaInfo.value(layer).at(1);
}

QString WmsCapabilities::legendUrl(const QString &layer)
{
    return m_wmsLayerMetaInfo.value(layer).at(2);
}

QString WmsCapabilities::style(const QString &layer)
{
    return m_wmsLayerMetaInfo.value(layer).at(3);
}

QStringList WmsCapabilities::styles(const QStringList &layers)
{
    QStringList retVal;
    for (auto layer : layers) {
        retVal << style(layer);
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

void WmsCapabilities::setWmsLayerMetaInfo(const QMap<QString, QStringList> &wmsLayerMetaInfo)
{
    m_wmsLayerMetaInfo = wmsLayerMetaInfo;
}

QMap<QString, QStringList> WmsCapabilities::wmsLayerMetaInfo() const
{
    return m_wmsLayerMetaInfo;
}

void WmsCapabilities::setWmsLayerCoordinateSystems(const QMap<QString, QMap<QString, QString> > &wmsLayerCoordinateSystems)
{
    m_wmsLayerCoordinateSystems = wmsLayerCoordinateSystems;
}

QMap<QString, QMap<QString, QString> > WmsCapabilities::wmsLayerCoordinateSystems() const
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


OwsServiceManager::OwsServiceManager(QObject *parent) : QObject(parent)
{
    connect( &m_capabilitiesAccessManager, &QNetworkAccessManager::finished, this, &OwsServiceManager::parseWmsCapabilities );
    connect( &m_imageAccessManager, &QNetworkAccessManager::finished, this, &OwsServiceManager::parseImageResult );
}

void OwsServiceManager::queryWmsCapabilities(const QUrl& queryUrl)
{
    m_url = queryUrl;
    QUrl url(queryUrl);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem( "service", "WMS" );
    urlQuery.addQueryItem( "request", "GetCapabilities" );
    url.setQuery(urlQuery);

    QNetworkRequest request;
    request.setUrl( url );
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    mDebug() << "for url" << url;
    m_capabilitiesAccessManager.get( request );
}

void OwsServiceManager::queryWmsMap(const QUrl &url, const QString &layers, const QString &projection,
                                    const QString &bbox, const QString &format, const QString &style)
{
    m_imageRequestResult.setResultImage(QImage());
    m_imageRequestResult.setImageStatus(WmsImageNone);
    m_imageRequestResult.setResultRaw("");
    m_imageRequestResult.setResultFormat(QString());

    QUrlQuery downloadQuery;
    downloadQuery.addQueryItem( "request", "GetMap" ); // Requests that the server generates a map.
    downloadQuery.addQueryItem( "service", "wms" ); // Service name. Value is WMS.
    downloadQuery.addQueryItem( "version", wmsCapabilities().version() ); // Service version. Value is one of 1.0.0, 1.1.0, 1.1.1, 1.3.0.
    downloadQuery.addQueryItem( "layers", layers ); // Layers to display on map. Value is a comma-separated list of layer names.

    // Spatial Reference System for map output. Value is in the form EPSG:nnn. srs was used before WMS 1.3.0, crs has been used since then.
    downloadQuery.addQueryItem( wmsCapabilities().referenceSystemType(), projection );
    // downloadQuery.addQueryItem( "bgcolor", "#ff0000" ); // rarely supported by servers

    downloadQuery.addQueryItem( "width", "256" ); // Width of map output, in pixels.
    downloadQuery.addQueryItem( "height", "256" ); // Height of map output, in pixels.

    QString boundingBox = bbox;
    if (boundingBox.isEmpty()) {
        if (projection == "epsg:3857") {
            boundingBox = "-20048966.1,-20037508.34,20048966.1,20037508.34";
        }
        else if (projection == "epsg:4326") {
            boundingBox = "-90,-180,90,180";
        }
    }
    downloadQuery.addQueryItem( "bbox", boundingBox );
    downloadQuery.addQueryItem( "transparent", "true");

    // Format for the map output. In addition to common bitmap formats WMS servers
    // sometimes support "vector" formats (PDF, SVG, KML, etc.)
    // Currently Marble only supports JPEG, PNG, TIFF, GIF, BMP and their variants.
    downloadQuery.addQueryItem( "format", QString("image/%1").arg(format) );

    // Styles in which layers are to be rendered. Value is a comma-separated list of style names,
    // or empty if default styling is required. Style names may be empty in the list,
    // to use default layer styling. However some servers do not accept empty style names.
    downloadQuery.addQueryItem( "styles", style );
    m_imageRequestResult.setResultFormat((format == QLatin1String("jpeg")) ? "jpg" : format); // Is this needed here?

    QUrl finalDownloadUrl( url );
    finalDownloadUrl.setQuery( downloadQuery );
    mDebug() << "requesting WMS image" << finalDownloadUrl;

    QNetworkRequest request( finalDownloadUrl );

    m_imageAccessManager.get( request );
}

void OwsServiceManager::queryWmsLevelZeroTile(const QUrl& url, const QString &layers, const QString &projection,
                                           const QString &format, const QString &style)
{
    QString bbox;
    if (projection == "epsg:3857") {
        bbox = "-20048966.1,-20037508.34,20048966.1,20037508.34";
    }
    else if (projection == "epsg:4326") {
        bbox = "-90,-180,90,180";
    }

    m_imageRequestResult.setResultType(LevelZeroTile);

    queryWmsMap(url, layers, projection, bbox, format, style);
}

void OwsServiceManager::queryWmsPreviewImage(const QUrl& url, const QString &layers, const QString &projection,
                                          const QString &format, const QString &style)
{

    QString firstLayer = layers.contains(',') ? layers.section(',',0,0) : layers;
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

    QNetworkRequest request( url );

    m_imageAccessManager.get( request );
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
    baseUrl.replace(baseUrl.indexOf(QLatin1String("{x}")), 3,  QString::number(0));
    baseUrl.replace(baseUrl.indexOf(QLatin1String("{y}")), 3,  QString::number(0));
    baseUrl.replace(baseUrl.indexOf(QLatin1String("{zoomLevel}")), 11,  QString::number(0));
    downloadUrl.setUrl( baseUrl );

    QNetworkRequest request( downloadUrl );
    mDebug() << "requesting static map" << downloadUrl;
    m_imageAccessManager.get( request );
}

WmsCapabilities Marble::OwsServiceManager::wmsCapabilities()
{
    return m_capabilities;
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

void OwsServiceManager::parseWmsCapabilities(QNetworkReply *reply)
{
    mDebug() << "received reply from" << reply->url();
    QString result( reply->readAll() );

    m_capabilities = WmsCapabilities(); // clear()
    QDomDocument xml;
    if( !xml.setContent( result ) )
    {
        m_capabilities.setCapabilitiesStatus(WmsCapabilitiesReplyUnreadable); // Wizard cannot parse server's response
        emit wmsCapabilitiesReady();
        return;
    }

    if( xml.documentElement().firstChildElement().tagName().isNull()
        || xml.documentElement().firstChildElement( "Capability" ).isNull() )
    {
        m_capabilities.setCapabilitiesStatus(WmsCapabilitiesNoWmsServer); // Server is not a Web Map Server.
        emit wmsCapabilitiesReady();
        return;
    }

    m_capabilities.setVersion( xml.documentElement().attribute("version") );
    m_capabilities.setReferenceSystemType( (m_capabilities.version() == "1.0.0"
                                         || m_capabilities.version() == "1.1.0"
                                         || m_capabilities.version() == "1.1.1")  ? "SRS" : "CRS" );

    QDomElement globalLayer = xml.documentElement().firstChildElement( "Capability" ).firstChildElement( "Layer" );
    QDomElement service = xml.documentElement().firstChildElement( "Service" );
//    QDomNodeList layers = globalLayer.elementsByTagName( "Layer" );
    QDomNodeList layers = xml.documentElement().firstChildElement( "Capability" ).elementsByTagName("Layer");

    m_capabilities.setTitle(service.firstChildElement( "Title" ).text());
    m_capabilities.setAbstract(service.firstChildElement( "Abstract" ).text() );

    QString contactEmail = service.firstChildElement( "ContactInformation").firstChildElement("ContactElectronicMailAddress" ).text();
    m_capabilities.setContactInformation(contactEmail);
    QString fees = service.firstChildElement( "Fees" ).text();
    m_capabilities.setFees(fees);

    QMap<QString, QStringList> wmsLayerMetaInfo;

    for( int i = 0; i < layers.size(); ++i )
    {
        QString name = layers.at(i).firstChildElement( "Name" ).text();
        QString title = layers.at(i).firstChildElement( "Title" ).text();
        QString abstract = layers.at(i ).firstChildElement( "Abstract" ).text();
        QDomElement legendElement = layers.at( i ).firstChildElement( "Style" ).firstChildElement( "LegendURL" );
        QString legendUrl;
        if (!legendElement.isNull()) legendUrl = legendElement.firstChildElement( "OnlineResource" ).attribute( "xlink:href" );
        QString style = layers.at(i).firstChildElement("Style").firstChildElement( "Name" ).text();
        if (style.isEmpty()) style = "default";

        wmsLayerMetaInfo[ name ] << title << abstract << legendUrl << style;
    }

    m_capabilities.setWmsLayerMetaInfo(wmsLayerMetaInfo);

    QMap<QString, QMap<QString, QString>> wmsLayerCoordinateSystems;
    for( int i = 0; i < layers.size(); ++i )
    {
        QString layerName = layers.at(i).firstChildElement( "Name" ).text();
        QDomNodeList projectionList = layers.at(i).toElement().elementsByTagName(m_capabilities.referenceSystemType());
        QDomNodeList layerPreviewBBox = layers.at(i).toElement().elementsByTagName("BoundingBox");

        for ( int s = 0; s < projectionList.size(); ++s ) {
            QString projection = projectionList.at(s).toElement().text().toLower();
            // SRS and CRS tags might contain a list of epsgs, so we need to use contains()
            if (projection.contains("epsg:3857")) {
                wmsLayerCoordinateSystems[layerName]["epsg:3857"] = QString();
            }
            if (projection.contains("epsg:4326")) {
                wmsLayerCoordinateSystems[layerName]["epsg:4326"] = QString();
            }
        }
        for ( int b = 0; b < layerPreviewBBox.size(); ++b ) {
            QDomElement bboxElement = layerPreviewBBox.at(b).toElement();
            QString bboxProjection = bboxElement.attribute(m_capabilities.referenceSystemType()).toLower();
            if (bboxProjection != "epsg:3857" && bboxProjection != "epsg:4326") continue;
            int precision = bboxProjection == "epsg:3857" ? 6 : 12;
            double west = bboxElement.attribute("minx").toDouble();
            double south = bboxElement.attribute("miny").toDouble();
            double east = bboxElement.attribute("maxx").toDouble();
            double north = bboxElement.attribute("maxy").toDouble();
            QString bboxString = QString("%1,%2,%3,%4")
            .arg(QString::number(west, 'f', precision),
                 QString::number(south, 'f', precision),
                 QString::number(east, 'f', precision),
                 QString::number(north, 'f', precision));
//          TODO: convert bbox coordinates from UTM to 3857 (e.g. from epsg:25832/33)
            wmsLayerCoordinateSystems[layerName][bboxProjection] = bboxString;
        }
        // FIXME: parse EX_GeographicBoundingBox if wmsLayerCoordinateSystems[layerName]["epsg:4326"/"epsg:3857"] == QString()

    }


    m_capabilities.setWmsLayerCoordinateSystems(wmsLayerCoordinateSystems);

    QDomNodeList formatList = xml.documentElement().firstChildElement( "Capability" ).firstChildElement( "Request" )
            .firstChildElement( "GetMap" ).elementsByTagName("Format");

    QStringList formats;
    for ( int f = 0; f < formatList.size(); ++f ) {
        QString format = formatList.at(f).toElement().text();
        format = format.right(format.length() - format.indexOf(QLatin1Char('/')) - 1).toLower();
        if (format == "jpeg" || format.contains("png")
            || format.contains("tif") || format.contains("gif")
            || format.contains("bmp") || format.contains("jpg") ) {
            formats << format;
        }
    }

    m_capabilities.setFormats(formats);

    m_capabilities.setCapabilitiesStatus(WmsCapabilitiesSuccess);
    emit wmsCapabilitiesReady();
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
    }
    else if (m_imageRequestResult.resultRaw().isNull()) {
        m_imageRequestResult.setImageStatus(WmsImageFailed); // Image could not be downloaded
    }
    else {
        m_imageRequestResult.setImageStatus(WmsImageSuccess);
    }

    m_imageRequestResult.setResultImage(testImage);

    QByteArray resultRaw = m_imageRequestResult.resultRaw();
    QBuffer testBuffer( &resultRaw );
    m_imageRequestResult.setResultFormat(QImageReader( &testBuffer ).format());

    emit imageRequestResultReady();
}

}

#include "moc_OwsServiceManager.cpp"
#include "OwsServiceManager.moc" // needed for Q_OBJECT here in source
