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
        result = m_coordinateSystems.value(projection);
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

void WmsCapabilities::setReferenceSystemType(const QString &refSystem)
{
    m_referenceSystemType = refSystem;
}

QString WmsCapabilities::referenceSystemType() const
{
    return m_referenceSystemType;
}

void WmsCapabilities::setCoordinateSystems(const QMap<QString, QString> &coordinateSystems)
{
    m_coordinateSystems = coordinateSystems;
}

QMap<QString, QString> WmsCapabilities::coordinateSystems() const
{
    return m_coordinateSystems;
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

void OwsServiceManager::queryWmsMap(const QUrl &url, const QString &layer, const QString &projection,
                                    const QString &bbox, const QString &format, const QString &style)
{
    m_imageRequestResult.setResultImage(QImage());
    m_imageRequestResult.setImageStatus(WmsImageNone);
    m_imageRequestResult.setResultRaw("");
    m_imageRequestResult.setResultFormat(QString());

    QUrl finalDownloadUrl( url );
    QUrlQuery downloadUrl;
    downloadUrl.addQueryItem( "request", "GetMap" );
    downloadUrl.addQueryItem( "version", "1.1.1" );
    downloadUrl.addQueryItem( "layers", layer );

    downloadUrl.addQueryItem( "srs", projection );


    downloadUrl.addQueryItem( "width", "256" );
    downloadUrl.addQueryItem( "height", "256" );

    QString boundingBox = bbox;
    if (boundingBox.isEmpty()) {
        if (projection == "epsg:3857") {
            boundingBox = "-20037508.34,-20048966.1,20037508.34,20048966.1";
        }
        else if (projection == "epsg:4326") {
            boundingBox = "-180,-90,180,90";
        }
    }
    downloadUrl.addQueryItem( "bbox", boundingBox );

    downloadUrl.addQueryItem( "format", QString("image/%1").arg(format) );
    downloadUrl.addQueryItem( "styles", style );
    m_imageRequestResult.setResultFormat((format == QLatin1String("jpeg")) ? "jpg" : format); // Is this needed here?

    finalDownloadUrl.setQuery( downloadUrl );
    mDebug() << "requesting WMS" << finalDownloadUrl;

    QNetworkRequest request( finalDownloadUrl );

    m_imageAccessManager.get( request );
}

void OwsServiceManager::queryWmsLevelZeroTile(const QUrl& url, const QString &layer, const QString &projection,
                                           const QString &format, const QString &style)
{
    QString bbox;
    if (projection == "epsg:3857") {
        bbox = "-20037508.34,-20048966.1,20037508.34,20048966.1";
    }
    else if (projection == "epsg:4326") {
        bbox = "-180,-90,180,90";
    }

    m_imageRequestResult.setResultType(LevelZeroTile);

    queryWmsMap(url, layer, projection, bbox, format, style);
}

void OwsServiceManager::queryWmsPreviewImage(const QUrl& url, const QString &layer, const QString &projection,
                                          const QString &format, const QString &style)
{
    QString bbox = wmsCapabilities().boundingBox(layer, projection);

    m_imageRequestResult.setResultType(PreviewImage);

    queryWmsMap(url, layer, projection, bbox, format, style);
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

    if( xml.documentElement().firstChildElement().tagName().isNull() )
    {
        m_capabilities.setCapabilitiesStatus(WmsCapabilitiesNoWmsServer); // Server is not a Web Map Server.
        emit wmsCapabilitiesReady();
        return;
    }

    m_capabilities.setVersion( xml.documentElement().attribute("version") );
    m_capabilities.setReferenceSystemType( m_capabilities.version() == "1.1.1" ? "SRS" : "CRS" );

    QDomElement globalLayer = xml.documentElement().firstChildElement( "Capability" ).firstChildElement( "Layer" );
    QDomElement service = xml.documentElement().firstChildElement( "Service" );
    QDomNodeList layers = globalLayer.elementsByTagName( "Layer" );

    m_capabilities.setTitle(service.firstChildElement( "Title" ).text());
    m_capabilities.setAbstract(service.firstChildElement( "Abstract" ).text() );

    QString contactEmail = service.firstChildElement( "ContactInformation").firstChildElement("ContactElectronicMailAddress" ).text();
    m_capabilities.setContactInformation(contactEmail);
    QString fees = service.firstChildElement( "Fees" ).text();
    m_capabilities.setFees(fees);

    QDomNodeList srsList = globalLayer.elementsByTagName(m_capabilities.referenceSystemType());
    QStringList projections;
    QMap<QString, QString> coordinateSystems;
    QDomNodeList previewBBoxList = globalLayer.elementsByTagName("BoundingBox");

    // If there is no bbox (optional in 1.1.0) then collect the projections directly.
    for (int s = 0; s < srsList.size(); ++s) {
        if (srsList.at(s).parentNode() == globalLayer && srsList.at(s).toElement().text().toLower() == "epsg:3857") {
            coordinateSystems["epsg:3857"] = QString();
        }
        else if (srsList.at(s).parentNode() == globalLayer && srsList.at(s).toElement().text().toLower() == "epsg:4326") {
            coordinateSystems["epsg:4326"] = QString();
        }
    }
    // Otherwise get projections and bbox right from the BoundingBox tags (mandatory in 1.3.0)
    for ( int b = 0; b < previewBBoxList.size(); ++b ) {
        if (previewBBoxList.at(b).parentNode() != globalLayer) continue;
        QDomElement bboxElement = previewBBoxList.at(b).toElement();
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
        coordinateSystems[bboxProjection] = bboxString;
    }

    m_capabilities.setCoordinateSystems(coordinateSystems);

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

        wmsLayerMetaInfo[ name ] << title << abstract << legendUrl << style;
    }


    m_capabilities.setWmsLayerMetaInfo(wmsLayerMetaInfo);

    QMap<QString, QMap<QString, QString>> wmsLayerCoordinateSystems;

    for( int i = 0; i < layers.size(); ++i )
    {
        QString layerName = layers.at(i).toElement().text();
        QDomNodeList projectionList = layers.at(i).toElement().elementsByTagName(m_capabilities.referenceSystemType());
        QDomNodeList layerPreviewBBox = layers.at(i).toElement().elementsByTagName("BoundingBox");

        // If there is no bbox (optional in 1.1.0) then collect the projections directly.
        if (layerPreviewBBox.isEmpty()) {
            for ( int s = 0; s < projectionList.size(); ++s ) {
                QString projection = projectionList.at(s).toElement().text().toLower();
                if (projection == "epsg:3857" || projection == "epsg:4326") {
                    wmsLayerCoordinateSystems[layerName][projection] = QString();
                }
            }
        }
        // Otherwise get projections and bbox right from the BoundingBox tags (mandatory in 1.3.0)
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
            wmsLayerCoordinateSystems[layerName][bboxProjection] = bboxString;
        }
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
