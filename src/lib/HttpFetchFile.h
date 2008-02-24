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

#include <QtCore/QBuffer>
#include <QtCore/QDebug>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUrl>

enum  Priority { NoPriority, Low, Medium, High };
enum  Status   { NoStatus, Pending, Activated, Finished, Expired, Aborted };

class QHttp;
class StoragePolicy;

class HttpJob
{
 public:
    HttpJob()
      : status( NoStatus ),
        priority( NoPriority )
    {
        buffer = new QBuffer( &data );
        buffer->open( QIODevice::WriteOnly );
    }

    virtual ~HttpJob()
    {
        buffer->close();
        delete buffer;
    }

    QString     originalRelativeUrlString;
    QString     relativeUrlString;
    QString     targetDirString;
    QUrl        serverUrl;
    QByteArray  data;
    QBuffer    *buffer;
    QString     initiatorId;
    Status      status;
    Priority    priority;
};


class HttpFetchFile : public QObject
{
    Q_OBJECT

 public:
    /**
     * Creates a new http fetch file object.
     */
    explicit HttpFetchFile( StoragePolicy *policy, QObject* parent = 0 );

    /**
     * Destroys the http fetch file object.
     */
    ~HttpFetchFile();

 public Q_SLOTS:
    void executeJob( HttpJob* job );

 Q_SIGNALS:
    void jobDone( HttpJob*, int );
    void statusMessage( QString );

 private Q_SLOTS:
    // process feedback from m_Http
    void httpRequestFinished( int requestId, bool error );

 private:
    QHttp *m_pHttp;
    QMap<int, HttpJob*> m_pJobMap;
    StoragePolicy *m_storagePolicy;
};


#endif // HTTPFETCHFILE_H
