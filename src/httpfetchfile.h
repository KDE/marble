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

#include <QtCore/QFile>
#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtNetwork/QHttp>
#include <QtNetwork/QHttpResponseHeader>


/**
@author Torsten Rahn
*/


class HttpFetchFile : public QObject
{
    Q_OBJECT

 public:
    HttpFetchFile( QObject* parent = 0 );

    void setServerUrl( const QString& serverUrl ){ m_serverUrl = serverUrl; }
    void setTargetDir( const QString& targetDir ){ m_targetDir = targetDir; }

 public slots:
    void downloadFile( const QUrl& );
    void cancelDownload();

 signals:
    void downloadDone( QString, bool );
    void statusMessage( QString );

 private slots:
    // process feedback from m_Http
    void httpRequestFinished(int requestId, bool error);
    void checkResponseHeader(const QHttpResponseHeader &responseHeader);

 private:
    QHttp   *m_pHttp;
    QFile   *m_pFile;
    int      m_httpGetId;
    bool     m_httpRequestAborted;

    QMap <int, QString>  m_pFileIdMap;

    QString  m_serverUrl;
    QString  m_targetDir;
};


#endif // HTTPFETCHFILE_H
