//
// C++ Interface: httpfetch
//
// Description: httpfetch

// The HttpFetchFile class downloads files.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution


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
