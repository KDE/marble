//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
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
    HttpJob( const QUrl & sourceUrl, const QString & destFileName, QString const id );
    ~HttpJob();

    QUrl sourceUrl() const;
    void setSourceUrl( const QUrl & );

    QString initiatorId() const;
    void setInitiatorId( const QString & );

    QString destinationFileName() const;
    void setDestinationFileName( const QString & );

    QString originalDestinationFileName() const;

    void setStatus( const Status );

    QBuffer * buffer();
    QByteArray & data();

 private:
    QUrl        m_sourceUrl;
    QString     m_destinationFileName;
    // if there is a redirection, we have to know the original file name
    // for proper blacklisting etc.
    QString     m_originalDestinationFileName;
    QByteArray  m_data;
    QBuffer    *m_buffer;
    QString     m_initiatorId;
    Status      m_status;
    Priority    m_priority;
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


inline QUrl HttpJob::sourceUrl() const
{
    return m_sourceUrl;
}

inline void HttpJob::setSourceUrl( const QUrl & url )
{
    m_sourceUrl = url;
}

inline QString HttpJob::initiatorId() const
{
    return m_initiatorId;
}

inline void HttpJob::setInitiatorId( const QString & id )
{
    m_initiatorId = id;
}

inline QString HttpJob::destinationFileName() const
{
    return m_destinationFileName;
}

inline void HttpJob::setDestinationFileName( const QString & fileName )
{
    m_destinationFileName = fileName;
}

inline QString HttpJob::originalDestinationFileName() const
{
    return m_originalDestinationFileName;
}

inline void HttpJob::setStatus( const Status status )
{
    m_status = status;
}

inline QBuffer * HttpJob::buffer()
{
    return m_buffer;
}

inline QByteArray & HttpJob::data()
{
    return m_data;
}


#endif // HTTPFETCHFILE_H
