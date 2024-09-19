// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2008, 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
// SPDX-FileCopyrightText: 2008 Pino Toscano <pino@kde.org>
//

#ifndef MARBLE_HTTPJOB_H
#define MARBLE_HTTPJOB_H

#include <QNetworkReply>
#include <QObject>

#include "MarbleGlobal.h"

#include "marble_export.h"

class QNetworkAccessManager;
class QString;
class QByteArray;
class QUrl;

namespace Marble
{
class HttpJobPrivate;

class MARBLE_EXPORT HttpJob : public QObject
{
    Q_OBJECT

public:
    HttpJob(const QUrl &sourceUrl, const QString &destFileName, const QString &id, QNetworkAccessManager *networkAccessManager);
    ~HttpJob() override;

    QUrl sourceUrl() const;
    void setSourceUrl(const QUrl &);

    QString initiatorId() const;
    void setInitiatorId(const QString &);

    QString destinationFileName() const;
    void setDestinationFileName(const QString &);

    bool tryAgain();

    DownloadUsage downloadUsage() const;
    void setDownloadUsage(const DownloadUsage);

    void setUserAgentPluginId(const QString &pluginId) const;

    QByteArray userAgent() const;

Q_SIGNALS:
    /**
     * errorCode contains 0, if there was no error and 1 otherwise
     */
    void jobDone(HttpJob *, int errorCode);
    void redirected(HttpJob *job, const QUrl &redirectionTarget);

    /**
     * This signal is emitted if the data was successfully received and
     * the argument data contains completely the downloaded content.
     */
    void dataReceived(HttpJob *job, const QByteArray &data);

public Q_SLOTS:
    void execute();

private Q_SLOTS:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void error(QNetworkReply::NetworkError code);
    void finished();

private:
    Q_DISABLE_COPY(HttpJob)
    HttpJobPrivate *const d;
    friend class HttpJobPrivate;
};

}

#endif
