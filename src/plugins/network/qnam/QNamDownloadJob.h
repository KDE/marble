//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#ifndef MARBLE_QNAM_DOWNLOAD_JOB_H
#define MARBLE_QNAM_DOWNLOAD_JOB_H

#include <QtNetwork/QNetworkReply>

#include "HttpJob.h"


class QNetworkAccessManager;

namespace Marble
{

class QNamDownloadJob: public HttpJob
{
    Q_OBJECT

 public:
    QNamDownloadJob( const QUrl & sourceUrl, const QString & destFileName,
                     const QString & id, QNetworkAccessManager * const );

    // HttpJob abstract method
    virtual void execute();

 public Q_SLOTS:
    void downloadProgress( qint64 bytesReceived, qint64 bytesTotal );
    void error( QNetworkReply::NetworkError code );
    void finished();

 private:
    QNetworkAccessManager * m_networkAccessManager;
    QNetworkReply * m_networkReply;
};

}

#endif
