//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// The HttpFetchFile class downloads files.
//


#ifndef HTTPFETCHFILE_H
#define HTTPFETCHFILE_H

#include <QtGlobal>
#include <QtCore/QFile>
#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtNetwork/QHttp>
#include <QtNetwork/QHttpResponseHeader>

enum  Priority { NoPriority, Low, Medium, High };
enum  Status   { NoStatus, Pending, Activated, Finished, Expired, Aborted };


struct HttpJob
{
    HttpJob()
    {
        status   = NoStatus;
        priority = NoPriority;
    }
    virtual ~HttpJob(){ /* NONE */ }

    QString  relativeUrlString;

    QUrl     serverUrl;

    QString  targetDirString;
    QFile*   targetFile;

    qint64   initiatorId;
    Priority priority;
    Status   status;
};


class HttpFetchFile : public QObject
{
    Q_OBJECT

 public:
    HttpFetchFile( QObject* parent = 0 );

    void setServerUrl( const QUrl& serverUrl ){ m_serverUrl = serverUrl; }
    void setTargetDir( const QString& targetDirString ){ m_targetDirString = targetDirString; }

 public Q_SLOTS:
    void executeJob( HttpJob* job );
/*
    void downloadUrl( const QUrl& serverUrl, const QString& relativeUrlString );
    void downloadFile( const QUrl& );
*/
    void cancelJob( HttpJob* job );

 Q_SIGNALS:
    void jobDone( HttpJob*, bool );
//    void downloadDone( QString, bool );
    void statusMessage( QString );

 private Q_SLOTS:
    // process feedback from m_Http
    void httpRequestFinished(int requestId, bool error);
//    void checkResponseHeader(const QHttpResponseHeader &responseHeader);

 private:
    QHttp   *m_pHttp;
    QFile   *m_pFile;
    bool     m_httpRequestAborted;

    QMap <int, HttpJob*>  m_pFileIdMap;

    QUrl  m_serverUrl;
    QString  m_targetDirString;
};


#endif // HTTPFETCHFILE_H
