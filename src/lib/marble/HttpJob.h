//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008,2009 Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2008      Pino Toscano <pino@kde.org>
//

#ifndef MARBLE_HTTPJOB_H
#define MARBLE_HTTPJOB_H

#include <QObject>
#include <QNetworkReply>

#include "MarbleGlobal.h"

#include "marble_export.h"

class QNetworkAccessManager;
class QString;
class QByteArray;
class QUrl;

namespace Marble
{
class HttpJobPrivate;

class MARBLE_EXPORT HttpJob: public QObject
{
    Q_OBJECT

 public:
    HttpJob( const QUrl & sourceUrl, const QString & destFileName, const QString &id, QNetworkAccessManager *networkAccessManager );
    ~HttpJob();

    QUrl sourceUrl() const;
    void setSourceUrl( const QUrl & );

    QString initiatorId() const;
    void setInitiatorId( const QString & );

    QString destinationFileName() const;
    void setDestinationFileName( const QString & );

    bool tryAgain();

    DownloadUsage downloadUsage() const;
    void setDownloadUsage( const DownloadUsage );

    void setUserAgentPluginId( const QString & pluginId ) const;

    QByteArray userAgent() const;

 Q_SIGNALS:
    /**
     * errorCode contains 0, if there was no error and 1 otherwise
     */
    void jobDone( HttpJob *, int errorCode );
    void redirected( HttpJob * job, const QUrl& redirectionTarget );

    /**
     * This signal is emitted if the data was successfully received and
     * the argument data contains completely the downloaded content.
     */
    void dataReceived( HttpJob * job, const QByteArray& data );

 public Q_SLOTS:
    void execute();

private Q_SLOTS:
   void downloadProgress( qint64 bytesReceived, qint64 bytesTotal );
   void error( QNetworkReply::NetworkError code );
   void finished();

 private:
    Q_DISABLE_COPY( HttpJob )
    HttpJobPrivate *const d;
    friend class HttpJobPrivate;
};

}

#endif
