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


#ifndef QHTTPNETWORKPLUGIN_H
#define QHTTPNETWORKPLUGIN_H

#include "NetworkPlugin.h"
#include "HttpJob.h"

#include <QtCore/QByteArray>
#include <QtCore/QString>

class QBuffer;
class QHttp;
class QUrl;

namespace Marble
{

class QHttpHttpJob: public HttpJob
{
    Q_OBJECT

 public:
    QHttpHttpJob( const QUrl & sourceUrl, const QString & destFileName, const QString & id );
    ~QHttpHttpJob();

    QByteArray & data();

 public Q_SLOTS:
    virtual void execute();

 private Q_SLOTS:
    void httpRequestFinished( int requestId, bool error );

 private:
    QByteArray  m_data;
    QBuffer    *m_buffer;
    QHttp       *m_http; // FIXME: cleans this up after 4.1
    int m_currentRequest;
};

inline QByteArray & QHttpHttpJob::data()
{
    return m_data;
}


class QHttpNetworkPlugin: public NetworkPlugin
{
    Q_OBJECT

 public:
    virtual QString name() const;
    virtual QString nameId() const;
    virtual QString guiString() const;
    virtual QString description() const;
    virtual QIcon icon() const;
    virtual void initialize();
    virtual bool isInitialized() const;

    virtual NetworkPlugin * newInstance() const;
    virtual HttpJob *createJob( const QUrl &source, const QString &destination, const QString &id );
};

}

#endif // QHTTPNETWORKPLUGIN_H
