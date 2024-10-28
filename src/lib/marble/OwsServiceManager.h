// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2023 Torsten Rahn <rahn@kde.org>
//

#ifndef OWSSERVICEMANAGER_H
#define OWSSERVICEMANAGER_H

#include <QDomDocument>
#include <QImage>
#include <QNetworkAccessManager>
#include <QObject>

class QNetworkReply;

namespace Marble
{

enum OwsServiceType {
    NoOwsType,
    WmsType,
    WmtsType,
    WfsType,
    WcsType
};

enum OwsCapabilitiesStatus {
    OwsCapabilitiesNone,
    OwsCapabilitiesSuccess,
    OwsCapabilitiesNoOwsServer,
    OwsCapabilitiesReplyUnreadable,
    OwsCapabilitiesFailed
};

enum WmsImageStatus {
    WmsImageNone,
    WmsImageSuccess,
    WmsImageFailed,
    WmsImageFailedServerMessage
};

class OwsMappingCapabilities
{
public:
    OwsMappingCapabilities();

    void setVersion(const QString &version);
    QString version() const; // "1.1.1" or "1.3.0"

    void setTitle(const QString &title);
    QString title() const;

    void setAbstract(const QString &abstract);
    QString abstract() const;

    QStringList layers() const;

    QString title(const QString &layer);
    QString abstract(const QString &layer);
    QString style(const QString &layer);

    void setOwsLayerMetaInfo(const QMap<QString, QStringList> &wmsLayerMetaInfo);
    QMap<QString, QStringList> owsLayerMetaInfo() const;

protected:
    QString m_version;
    QString m_title;
    QString m_abstract;

    QMap<QString, QStringList> m_owsLayerMetaInfo; // layerName -> Title, Abstract, LegendUrl, Style
};

class WmsCapabilities : public OwsMappingCapabilities
{
public:
    WmsCapabilities();

    void setContactInformation(const QString &info);
    QString contactInformation() const;

    void setFees(const QString &fee);
    QString fees() const;

    QStringList projections(const QString &layer);
    QString boundingBox(const QString &layer, const QString &projection);
    QString legendUrl(const QString &layer);
    QStringList styles(const QStringList &layers);
    QString boundingBoxNSEWDegrees(const QStringList &layers, const QString &projection);

    void setReferenceSystemType(const QString &refSystem);
    QString referenceSystemType() const; // SRS (1.1.1) or CRS (1.3.0)

    void setWmsLayerCoordinateSystems(const QMap<QString, QMap<QString, QString>> &wmsLayerCoordinateSystems);
    QMap<QString, QMap<QString, QString>> wmsLayerCoordinateSystems() const;

    void setFormats(const QStringList &formats);
    QStringList formats();

    QString m_referenceSystemType;

    QString m_contactInformation;
    QString m_fees;

    QMap<QString, QMap<QString, QString>> m_wmsLayerCoordinateSystems; // layerId -> epsg -> bbox

    QStringList m_formats; // png, jpg, ...
};

class WmtsCapabilities : public OwsMappingCapabilities
{
public:
    WmtsCapabilities();

    void setWmtsTileMatrixSets(const QMap<QString, QStringList> &wmtsTileMatrixSets);
    QMap<QString, QStringList> wmtsTileMatrixSets() const;
    void setWmtsTileResource(const QMap<QString, QMap<QString, QString>> &wmtsTileResource);
    QMap<QString, QMap<QString, QString>> wmtsTileResource() const;

    QMap<QString, QStringList> m_wmtsTileMatrixSets; //  layerId -> TileMatrixSets
    QMap<QString, QMap<QString, QString>> m_wmtsTileResource; // layerId -> Format -> RessourceUrl
};

enum ImageResultType {
    GenericImage,
    LevelZeroTile,
    PreviewImage,
    LegendImage
};

class ImageRequestResult
{
public:
    ImageRequestResult();

    void setImageStatus(WmsImageStatus imageStatus);
    WmsImageStatus imageStatus() const;

    void setResultImage(const QImage &image);
    QImage resultImage() const;

    void setResultRaw(const QByteArray &resultRaw);
    QByteArray resultRaw() const;

    void setResultFormat(const QString &resultFormat);
    QString resultFormat() const;

    void setResultType(const ImageResultType);
    ImageResultType resultType() const;

private:
    WmsImageStatus m_imageStatus;
    QImage m_resultImage;
    QByteArray m_resultRaw;
    QString m_resultFormat;
    ImageResultType m_resultType;
};

class OwsServiceManager : public QObject
{
    Q_OBJECT
public:
    explicit OwsServiceManager(QObject *parent = nullptr);

public Q_SLOTS:
    void queryOwsCapabilities(const QUrl &queryUrl, const QString &serviceString = QStringLiteral("WMS"));
    void queryWmsMap(const QUrl &url,
                     const QString &layers,
                     const QString &projection,
                     const QString &bbox,
                     const QString &format,
                     const QString &styles = QString());
    void queryWmsLevelZeroTile(const QUrl &url, const QString &layers, const QString &projection, const QString &format, const QString &styles = QString());
    void queryWmsPreviewImage(const QUrl &url, const QString &layers, const QString &projection, const QString &format, const QString &styles = QString());
    void queryWmsLegendImage(const QUrl &url);

    void queryWmtsLevelZeroTile(const QString &url, const QString &style, const QString &tileMatrixSet);
    void queryWmtsPreviewImage(const QString &url, const QString &style, const QString &tileMatrixSet);
    void queryWmtsTile(const QString &url,
                       const QString &style,
                       const QString &tileMatrixSet,
                       const QString &tileMatrix,
                       const QString &tileRow,
                       const QString &tileCol);

    void queryXYZPreviewImage(const QString &urlString);
    void queryXYZLevelZeroTile(const QString &urlString);
    void queryXYZImage(const QString urlString);

    void handleAuthentication(QNetworkReply *reply, QAuthenticator *authenticator);

    void setCapabilitiesStatus(OwsCapabilitiesStatus capabilitiesStatus);
    OwsCapabilitiesStatus capabilitiesStatus() const;

    OwsServiceType owsServiceType();
    WmsCapabilities wmsCapabilities();
    WmtsCapabilities wmtsCapabilities();
    ImageRequestResult imageRequestResult();

    QImage resultImage();
    QByteArray resultRaw();
    QString resultFormat();
    ImageResultType resultType();

Q_SIGNALS:
    void wmsCapabilitiesReady();
    void wmtsCapabilitiesReady();
    void imageRequestResultReady();

private Q_SLOTS:
    void parseOwsCapabilities(QNetworkReply *reply);
    void parseWmsCapabilities(QNetworkReply *reply);
    void parseWmtsCapabilities(QNetworkReply *reply);
    void parseImageResult(QNetworkReply *reply);

private:
    QUrl m_url;
    QDomDocument m_xml;
    QNetworkAccessManager m_capabilitiesAccessManager;
    QNetworkAccessManager m_imageAccessManager;
    OwsCapabilitiesStatus m_capabilitiesStatus;
    OwsServiceType m_owsServiceType;
    WmsCapabilities m_wmsCapabilities;
    WmtsCapabilities m_wmtsCapabilities;
    ImageRequestResult m_imageRequestResult;
};

}

#endif // OWSSERVICEMANAGER_H
