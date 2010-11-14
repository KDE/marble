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

#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QUrl>

#include "global.h"

#include "marble_export.h"

namespace Marble
{
class HttpJobPrivate;

class MARBLE_EXPORT HttpJob: public QObject
{
    Q_OBJECT

 public:
    HttpJob( const QUrl & sourceUrl, const QString & destFileName, const QString &id );
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
    void redirected( HttpJob * job, QUrl redirectionTarget );

    /**
     * This signal is emitted if the data was successfully received and
     * the argument data contains completely the downloaded content.
     */
    void dataReceived( HttpJob * job, QByteArray data );

 public Q_SLOTS:
    virtual void execute() = 0;

 private:
    Q_DISABLE_COPY( HttpJob )
    HttpJobPrivate *const d;
    friend class HttpJobPrivate;
};

}

#endif
