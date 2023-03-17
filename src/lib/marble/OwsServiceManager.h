// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2023 Torsten Rahn <rahn@kde.org>
//

#ifndef OWSSERVICEMANAGER_H
#define OWSSERVICEMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QImage>

class QNetworkReply;

namespace Marble
{

enum WmsCapabilitiesStatus {
    WmsCapabilitiesNone,
    WmsCapabilitiesSuccess,
    WmsCapabilitiesNoWmsServer,
    WmsCapabilitiesReplyUnreadable,
    WmsCapabilitiesFailed
};

enum WmsImageStatus {
    WmsImageNone,
    WmsImageSuccess,
    WmsImageFailed,
    WmsImageFailedServerMessage
};

class WmsCapabilities {

public:
    WmsCapabilities();

    void setCapabilitiesStatus(WmsCapabilitiesStatus capabilitiesStatus);
    WmsCapabilitiesStatus capabilitiesStatus() const;

    void setVersion(const QString& version);
    QString version() const; // "1.1.1" or "1.3.0"

    void setTitle(const QString& title);
    QString title() const;

    void setAbstract(const QString& abstract);
    QString abstract() const;

    void setContactInformation(const QString& info);
    QString contactInformation() const;

    void setFees(const QString& fee);
    QString fees() const;

    QStringList layers() const;
    QStringList projections(const QString& layer);
    QString boundingBox(const QString& layer, const QString& projection);
    QString title(const QString& layer);
    QString abstract(const QString& layer);
    QString legendUrl(const QString& layer);
    QString style(const QString& layer);

    void setReferenceSystemType(const QString& refSystem);
    QString referenceSystemType() const; // SRS (1.1.1) or CRS (1.3.0)

    void setCoordinateSystems(const QMap<QString, QString>& coordinateSystems);
    QMap<QString, QString> coordinateSystems() const;

    void setWmsLayerMetaInfo(const QMap<QString, QStringList>& wmsLayerMetaInfo);
    QMap<QString, QStringList> wmsLayerMetaInfo() const;

    void setWmsLayerCoordinateSystems(const QMap<QString, QMap<QString, QString>>& wmsLayerCoordinateSystems);
    QMap<QString, QMap<QString, QString>> wmsLayerCoordinateSystems() const;

    void setFormats(const QStringList& formats);
    QStringList formats();

    WmsCapabilitiesStatus m_capabilitiesStatus;
    QString m_version;
    QString m_title;
    QString m_abstract;
    QString m_referenceSystemType;

    QString m_contactInformation;
    QString m_fees;

    QMap<QString, QString> m_coordinateSystems; // epsg -> bbox
    QMap<QString, QStringList> m_wmsLayerMetaInfo; // layerName -> Title, Abstract, LegendUrl, Style
    QMap<QString, QMap<QString, QString>> m_wmsLayerCoordinateSystems; // layerId -> epsg -> bbox

    QStringList m_formats; // png, jpg, ...
};

enum ImageResultType {
    GenericImage,
    LevelZeroTile,
    PreviewImage
};

class ImageRequestResult {
public:
    ImageRequestResult();

    void setImageStatus(WmsImageStatus imageStatus);
    WmsImageStatus imageStatus() const;

    void setResultImage(const QImage& image);
    QImage resultImage() const;

    void setResultRaw(const QByteArray& resultRaw);
    QByteArray resultRaw() const;

    void setResultFormat(const QString& resultFormat);
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
    void queryWmsCapabilities(const QUrl& queryUrl);
    void queryWmsMap(const QUrl& url, const QString& layer, const QString& projection,
                     const QString& bbox, const QString& format, const QString& styles = QString());
    void queryWmsLevelZeroTile(const QUrl& url, const QString &layer, const QString &projection,
                            const QString &format, const QString &styles = QString());
    void queryWmsPreviewImage(const QUrl& url, const QString &layer, const QString &projection,
                           const QString &format, const QString &styles = QString());

    void queryXYZPreviewImage(const QString& urlString);
    void queryXYZLevelZeroTile(const QString& urlString);
    void queryXYZImage(const QString urlString);

    WmsCapabilities wmsCapabilities();
    ImageRequestResult imageRequestResult();

    QImage resultImage();
    QByteArray resultRaw();
    QString resultFormat();
    ImageResultType resultType();

Q_SIGNALS:
    void wmsCapabilitiesReady();
    void imageRequestResultReady();

private Q_SLOTS:
    void parseWmsCapabilities(QNetworkReply* reply);
    void parseImageResult(QNetworkReply* reply);

private:
    QUrl m_url;
    QNetworkAccessManager m_capabilitiesAccessManager;
    QNetworkAccessManager m_imageAccessManager;
    WmsCapabilities m_capabilities;
    ImageRequestResult m_imageRequestResult;
};

}

#endif // OWSSERVICEMANAGER_H
